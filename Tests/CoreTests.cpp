#include "../ArrayND.h"
#include "../FieldTypes.h"
#include "../FluidGrid.h"
#include "../Point.h"
#include "../PointVecOps.h"
#include "../ScalarField.h"
#include "../Tensor.h"
#include "../Vec.h"
#include "../World.h"

#include <array>
#include <cmath>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <type_traits>

namespace
{
int failed_expectations = 0;

void expectTrue(bool condition, const char* expression, const char* file, int line)
{
    if (!condition) {
        ++failed_expectations;
        std::cerr << file << ':' << line << ": expected " << expression << '\n';
    }
}

void expectFalse(bool condition, const char* expression, const char* file, int line)
{
    if (condition) {
        ++failed_expectations;
        std::cerr << file << ':' << line << ": expected not " << expression << '\n';
    }
}

template<typename T, typename U>
void expectEqual(const T& actual, const U& expected, const char* actual_expression, const char* expected_expression, const char* file, int line)
{
    if (!(actual == expected)) {
        ++failed_expectations;
        std::cerr << file << ':' << line << ": expected " << actual_expression << " == " << expected_expression
            << " (" << actual << " vs " << expected << ")\n";
    }
}

void expectNear(double actual, double expected, double tolerance, const char* actual_expression, const char* expected_expression, const char* file, int line)
{
    if (std::fabs(actual - expected) > tolerance) {
        ++failed_expectations;
        std::cerr << file << ':' << line << ": expected " << actual_expression << " ~= " << expected_expression
            << " (" << actual << " vs " << expected << ", tolerance " << tolerance << ")\n";
    }
}

template<typename ExceptionType, typename Func>
void expectThrows(Func&& func, const char* expression, const char* exception_name, const char* file, int line)
{
    try {
        func();
    }
    catch (const ExceptionType&) {
        return;
    }
    catch (const std::exception& exception) {
        ++failed_expectations;
        std::cerr << file << ':' << line << ": expected " << expression << " to throw " << exception_name
            << ", but caught " << exception.what() << '\n';
        return;
    }
    catch (...) {
        ++failed_expectations;
        std::cerr << file << ':' << line << ": expected " << expression << " to throw " << exception_name
            << ", but caught an unknown exception\n";
        return;
    }

    ++failed_expectations;
    std::cerr << file << ':' << line << ": expected " << expression << " to throw " << exception_name << '\n';
}

void runTest(const char* name, void (*test)())
{
    const int failures_before = failed_expectations;

    try {
        test();
    }
    catch (const std::exception& exception) {
        ++failed_expectations;
        std::cerr << "Unhandled exception in " << name << ": " << exception.what() << '\n';
    }
    catch (...) {
        ++failed_expectations;
        std::cerr << "Unhandled unknown exception in " << name << '\n';
    }

    if (failed_expectations == failures_before) {
        std::cout << "[PASS] " << name << '\n';
    }
    else {
        std::cout << "[FAIL] " << name << '\n';
    }
}

#define EXPECT_TRUE(expression) expectTrue((expression), #expression, __FILE__, __LINE__)
#define EXPECT_FALSE(expression) expectFalse((expression), #expression, __FILE__, __LINE__)
#define EXPECT_EQ(actual, expected) expectEqual((actual), (expected), #actual, #expected, __FILE__, __LINE__)
#define EXPECT_NEAR(actual, expected, tolerance) expectNear((actual), (expected), (tolerance), #actual, #expected, __FILE__, __LINE__)
#define EXPECT_THROWS(expression, exception_type) expectThrows<exception_type>([&] { (void)(expression); }, #expression, #exception_type, __FILE__, __LINE__)

void testVecConstructionAndAccess()
{
    using toolbox::Vec;

    Vec<int, 3> zero;
    EXPECT_EQ(zero.at(0), 0);
    EXPECT_EQ(zero[1], 0);
    EXPECT_EQ(zero[2], 0);

    Vec<int, 3> values{ 1, 2, 3 };
    EXPECT_EQ(values.at(0), 1);
    EXPECT_EQ(values[1], 2);
    values[2] = 9;
    EXPECT_EQ(values[2], 9);

    const std::array<int, 3> source{ 4, 5, 6 };
    const Vec<int, 3> from_array{ source };
    EXPECT_EQ(from_array[0], 4);
    EXPECT_EQ(from_array[2], 6);

    const Vec<int, 4> filled{ 7 };
    EXPECT_EQ(filled[0], 7);
    EXPECT_EQ(filled[3], 7);

    values.fill(11);
    EXPECT_EQ(values[0], 11);
    EXPECT_EQ(values[2], 11);

    int sum = 0;
    for (const auto component : Vec<int, 3>{ 1, 2, 3 }) {
        sum += component;
    }
    EXPECT_EQ(sum, 6);

    auto iterator = values.begin();
    *iterator = 12;
    EXPECT_EQ(values[0], 12);

    const Vec<int, 3> const_values{ 8, 9, 10 };
    EXPECT_EQ(*const_values.cbegin(), 8);
    EXPECT_EQ(*(const_values.cend() - 1), 10);

    std::ostringstream output;
    output << Vec<int, 3>{ 1, 2, 3 };
    EXPECT_EQ(output.str(), std::string("1 2 3"));
}

void testVecArithmetic()
{
    using toolbox::Vec;
    using toolbox::dot;

    static_assert(std::is_same_v<decltype(Vec<int, 2>{ 1, 2 } + Vec<double, 2>{ 0.5, 1.5 }), Vec<double, 2>>);

    const Vec<int, 3> left{ 1, 2, 3 };
    const Vec<int, 3> right{ 4, 6, 8 };

    const auto sum = left + right;
    EXPECT_EQ(sum[0], 5);
    EXPECT_EQ(sum[2], 11);

    const auto difference = right - left;
    EXPECT_EQ(difference[0], 3);
    EXPECT_EQ(difference[2], 5);

    const auto product = left * right;
    EXPECT_EQ(product[0], 4);
    EXPECT_EQ(product[2], 24);

    const auto scaled_right = left * 3;
    EXPECT_EQ(scaled_right[0], 3);
    EXPECT_EQ(scaled_right[2], 9);

    const auto scaled_left = 2 * left;
    EXPECT_EQ(scaled_left[0], 2);
    EXPECT_EQ(scaled_left[2], 6);

    const auto divided = right / 2;
    EXPECT_EQ(divided[0], 2);
    EXPECT_EQ(divided[2], 4);

    EXPECT_EQ(dot(left, right), 40);
    const Vec<int, 3> expected_left{ 1, 2, 3 };
    EXPECT_TRUE(left == expected_left);
    EXPECT_TRUE(left != right);

    auto mutated = left;
    mutated += Vec<int, 3>{ 1, 1, 1 };
    EXPECT_EQ(mutated[0], 2);
    EXPECT_EQ(mutated[2], 4);

    mutated -= Vec<int, 3>{ 2, 1, 0 };
    EXPECT_EQ(mutated[0], 0);
    EXPECT_EQ(mutated[2], 4);

    mutated *= 3;
    EXPECT_EQ(mutated[1], 6);
    EXPECT_EQ(mutated[2], 12);

    mutated /= 2;
    EXPECT_EQ(mutated[1], 3);
    EXPECT_EQ(mutated[2], 6);

    const auto negated = -left;
    EXPECT_EQ(negated[0], -1);
    EXPECT_EQ(negated[2], -3);
}

void testVecFloatingPointHelpers()
{
    using toolbox::Vec;
    using toolbox::unit_vector;

    Vec<double, 2> vector{ 3.0, 4.0 };
    EXPECT_NEAR(vector.length_squared(), 25.0, 1e-12);
    EXPECT_NEAR(vector.length(), 5.0, 1e-12);

    const auto normalized = vector.normalized();
    EXPECT_NEAR(normalized[0], 0.6, 1e-12);
    EXPECT_NEAR(normalized[1], 0.8, 1e-12);
    EXPECT_NEAR(vector[0], 3.0, 1e-12);

    vector.normalize();
    EXPECT_NEAR(vector.length(), 1.0, 1e-12);

    const auto unit = unit_vector(Vec<double, 2>{ 0.0, 2.0 });
    EXPECT_NEAR(unit[0], 0.0, 1e-12);
    EXPECT_NEAR(unit[1], 1.0, 1e-12);

    const Vec<double, 3> nearly_zero{ 1e-10, -1e-10, 0.0 };
    const Vec<double, 3> not_nearly_zero{ 1e-7, 0.0, 0.0 };
    EXPECT_TRUE(nearly_zero.near_zero());
    EXPECT_FALSE(not_nearly_zero.near_zero());

    Vec<double, 2> zero;
    zero.normalize();
    EXPECT_NEAR(zero[0], 0.0, 1e-12);
    EXPECT_NEAR(zero[1], 0.0, 1e-12);
}

void testPoint()
{
    using toolbox::Point;
    using toolbox::Vec;

    Point<int, 3> zero;
    EXPECT_EQ(zero[0], 0);
    EXPECT_EQ(zero.at(2), 0);

    Point<int, 3> point{ 1, 2, 3 };
    EXPECT_EQ(point.at(0), 1);
    point[1] = 9;
    EXPECT_EQ(point[1], 9);

    const std::array<int, 3> source{ 4, 5, 6 };
    const Point<int, 3> from_array{ source };
    EXPECT_EQ(from_array[0], 4);
    EXPECT_EQ(from_array[2], 6);

    point += Vec<int, 3>{ 1, -2, 3 };
    EXPECT_EQ(point[0], 2);
    EXPECT_EQ(point[1], 7);
    EXPECT_EQ(point[2], 6);

    int sum = 0;
    for (const auto coordinate : point) {
        sum += coordinate;
    }
    EXPECT_EQ(sum, 15);

    auto iterator = point.begin();
    *iterator = 12;
    EXPECT_EQ(point[0], 12);

    EXPECT_EQ(*from_array.cbegin(), 4);
    EXPECT_EQ(*(from_array.cend() - 1), 6);
}

void testPointVecOps()
{
    using toolbox::Point;
    using toolbox::Vec;

    const Point<int, 2> point{ 3, 4 };
    const Vec<int, 2> vector{ 10, -2 };

    const auto translated_forward = point + vector;
    EXPECT_EQ(translated_forward[0], 13);
    EXPECT_EQ(translated_forward[1], 2);

    const auto translated_backward = point - vector;
    EXPECT_EQ(translated_backward[0], -7);
    EXPECT_EQ(translated_backward[1], 6);

    const auto displacement = translated_forward - point;
    EXPECT_EQ(displacement[0], 10);
    EXPECT_EQ(displacement[1], -2);
}

void testFieldTypes()
{
    using toolbox::fieldtypes::CellCentered;
    using toolbox::fieldtypes::FaceCentered;
    using Cell3 = CellCentered<double, 3>;
    using FaceY3 = FaceCentered<double, 3, 1>;

    EXPECT_NEAR(Cell3::offset[0], 0.5, 1e-12);
    EXPECT_NEAR(Cell3::offset[2], 0.5, 1e-12);
    EXPECT_EQ(Cell3::index_extend[0], std::size_t{ 0 });
    EXPECT_EQ(Cell3::index_extend[2], std::size_t{ 0 });

    EXPECT_NEAR(FaceY3::offset[0], 0.5, 1e-12);
    EXPECT_NEAR(FaceY3::offset[1], 0.0, 1e-12);
    EXPECT_NEAR(FaceY3::offset[2], 0.5, 1e-12);
    EXPECT_EQ(FaceY3::index_extend[0], std::size_t{ 0 });
    EXPECT_EQ(FaceY3::index_extend[1], std::size_t{ 1 });
    EXPECT_EQ(FaceY3::index_extend[2], std::size_t{ 0 });
}

void testScalarFieldIndexBounds()
{
    using CellField = toolbox::ScalarField<double, 2, toolbox::fieldtypes::CellCentered<double, 2>>;
    using FaceYField = toolbox::ScalarField<double, 2, toolbox::fieldtypes::FaceCentered<double, 2, 1>>;
    using Index = toolbox::Vec<int, 2>;
    using Shape = toolbox::Vec<std::size_t, 2>;

    CellField cell_field{ Shape{ 4, 3 }, 1.0, std::size_t{ 2 } };
    EXPECT_EQ(cell_field.data().shape()[0], std::size_t{ 8 });
    EXPECT_EQ(cell_field.data().shape()[1], std::size_t{ 7 });

    EXPECT_TRUE(cell_field.isPhysical(Index{ 0, 0 }));
    EXPECT_TRUE(cell_field.isPhysical(Index{ 3, 2 }));
    EXPECT_FALSE(cell_field.isPhysical(Index{ -1, 0 }));
    EXPECT_FALSE(cell_field.isPhysical(Index{ 4, 2 }));
    EXPECT_FALSE(cell_field.isPhysical(Index{ 3, 3 }));

    EXPECT_TRUE(cell_field.isStored(Index{ -2, -2 }));
    EXPECT_TRUE(cell_field.isStored(Index{ 5, 4 }));
    EXPECT_FALSE(cell_field.isStored(Index{ -3, 0 }));
    EXPECT_FALSE(cell_field.isStored(Index{ 6, 0 }));
    EXPECT_FALSE(cell_field.isStored(Index{ 0, 5 }));

    cell_field.at(Index{ -2, -2 }) = 12.0;
    cell_field.at(Index{ 5, 4 }) = 24.0;
    cell_field(Index{ 0, 0 }) = 36.0;
    cell_field(3, 2) = 48.0;
    EXPECT_NEAR(cell_field.at(Index{ -2, -2 }), 12.0, 1e-12);
    EXPECT_NEAR(cell_field.at(Index{ 5, 4 }), 24.0, 1e-12);
    EXPECT_NEAR(cell_field(Index{ 0, 0 }), 36.0, 1e-12);
    EXPECT_NEAR(cell_field(3, 2), 48.0, 1e-12);
    EXPECT_THROWS(cell_field.at(Index{ 6, 0 }), std::out_of_range);

    cell_field.data().fill(2.0);
    EXPECT_NEAR(cell_field.sample(toolbox::Point<double, 2>{ 1.0, 1.0 }), 2.0, 1e-12);

    CellField small_cell_field{ Shape{ 4, 3 }, 0.05, std::size_t{ 1 } };
    const auto bottom_left_index = small_cell_field.positionToIndex(toolbox::Point<double, 2>{ 0.03, 0.03 });
    EXPECT_EQ(bottom_left_index[0], 0);
    EXPECT_EQ(bottom_left_index[1], 0);

    FaceYField face_field{ Shape{ 4, 3 }, 1.0, std::size_t{ 1 } };
    EXPECT_EQ(face_field.data().shape()[0], std::size_t{ 6 });
    EXPECT_EQ(face_field.data().shape()[1], std::size_t{ 6 });

    EXPECT_TRUE(face_field.isPhysical(Index{ 3, 3 }));
    EXPECT_FALSE(face_field.isPhysical(Index{ 4, 3 }));
    EXPECT_FALSE(face_field.isPhysical(Index{ 3, 4 }));

    EXPECT_TRUE(face_field.isStored(Index{ -1, -1 }));
    EXPECT_TRUE(face_field.isStored(Index{ 4, 4 }));
    EXPECT_FALSE(face_field.isStored(Index{ -2, 0 }));
    EXPECT_FALSE(face_field.isStored(Index{ 5, 0 }));
    EXPECT_FALSE(face_field.isStored(Index{ 0, 5 }));

    FaceYField left{ Shape{ 4, 3 }, 1.0, std::size_t{ 1 } };
    FaceYField right{ Shape{ 4, 3 }, 1.0, std::size_t{ 1 } };
    left.data().fill(1.0);
    right.data().fill(0.25);
    const auto difference = left - right;
    EXPECT_EQ(difference.data().shape()[0], left.data().shape()[0]);
    EXPECT_EQ(difference.data().shape()[1], left.data().shape()[1]);
    EXPECT_EQ(difference.data().size(), left.data().size());
    EXPECT_NEAR(difference.data()[0], 0.75, 1e-12);
    EXPECT_NEAR(difference.data()[difference.data().size() - 1], 0.75, 1e-12);
}

void testFluidGridInitializesBoundarySolids()
{
    FluidGrid<double> grid{ 5, 4, 1.0 };
    grid.initialize();

    EXPECT_TRUE(grid.cellTypeAt(0, 0) == CellType::Solid);
    EXPECT_TRUE(grid.cellTypeAt(4, 0) == CellType::Solid);
    EXPECT_TRUE(grid.cellTypeAt(0, 3) == CellType::Solid);
    EXPECT_TRUE(grid.cellTypeAt(4, 3) == CellType::Solid);
    EXPECT_TRUE(grid.cellTypeAt(2, 0) == CellType::Solid);
    EXPECT_TRUE(grid.cellTypeAt(2, 3) == CellType::Solid);
    EXPECT_TRUE(grid.cellTypeAt(0, 2) == CellType::Solid);
    EXPECT_TRUE(grid.cellTypeAt(4, 2) == CellType::Solid);

    EXPECT_TRUE(grid.cellTypeAt(1, 1) == CellType::Liquid);
    EXPECT_TRUE(grid.cellTypeAt(3, 2) == CellType::Liquid);

    EXPECT_TRUE(grid.cellTypeAt(-1, 1) == CellType::Solid);
    EXPECT_TRUE(grid.cellTypeAt(5, 1) == CellType::Solid);
}

void testWorldUpdateNearBottomBoundary()
{
    World world{ SimulationConfig{
        .num_particles = 1,
        .grid_width = 16,
        .grid_height = 12,
        .cell_size_m = 0.05,
        .particle_radius_m = 0.035
    } };

    world.add_particle(Particle{ 0.07, 0.07, 0.0, 0.0, world.config().particle_radius_m });
    world.update(0.033);

    EXPECT_EQ(world.particles().size(), std::size_t{ 1 });
    EXPECT_TRUE(world.particles()[0].position()[1] >= world.config().particle_radius_m);
}

void testWorldUpdateSeededLowerHalfFluid()
{
    World world{ SimulationConfig{
        .num_particles = 2000,
        .grid_width = 160,
        .grid_height = 120,
        .cell_size_m = 0.05,
        .particle_radius_m = 0.035
    } };

    const auto& config = world.config();
    const double radius = config.particle_radius_m;
    const double spacing = radius * 3.0;
    const double x_min = radius * 2.0;
    const double x_max = config.domain_width_m() - radius * 2.0;
    const double y_min = radius * 2.0;
    const double y_max = config.domain_height_m() * 0.5;

    std::size_t particles_added = 0;
    for (double y = y_min; y < y_max && particles_added < config.num_particles; y += spacing) {
        for (double x = x_min; x < x_max && particles_added < config.num_particles; x += spacing) {
            world.add_particle(Particle(x, y, 0.0, 0.0, radius));
            ++particles_added;
        }
    }

    EXPECT_EQ(world.particles().size(), config.num_particles);

    double max_abs_position = 0.0;
    double max_abs_velocity = 0.0;

    for (int frame = 0; frame < 10; ++frame) {
        try {
            world.update(0.033);
        }
        catch (const std::exception& exception) {
            throw std::runtime_error(
                "seeded lower half update failed at frame " + std::to_string(frame) +
                " (max_abs_position=" + std::to_string(max_abs_position) +
                ", max_abs_velocity=" + std::to_string(max_abs_velocity) + ")" +
                ": " + exception.what());
        }

        max_abs_position = 0.0;
        max_abs_velocity = 0.0;
        for (const auto& particle : world.particles()) {
            EXPECT_TRUE(std::isfinite(particle.position()[0]));
            EXPECT_TRUE(std::isfinite(particle.position()[1]));
            EXPECT_TRUE(std::isfinite(particle.velocity()[0]));
            EXPECT_TRUE(std::isfinite(particle.velocity()[1]));
            max_abs_position = std::max(max_abs_position, std::abs(particle.position()[0]));
            max_abs_position = std::max(max_abs_position, std::abs(particle.position()[1]));
            max_abs_velocity = std::max(max_abs_velocity, std::abs(particle.velocity()[0]));
            max_abs_velocity = std::max(max_abs_velocity, std::abs(particle.velocity()[1]));
        }
    }

    EXPECT_EQ(world.particles().size(), config.num_particles);
}

void testTensorConstructionAccessAndArithmetic()
{
    Tensor<int, 1, 0, 3> constructed{ 1, 2, 3 };
    EXPECT_EQ(constructed[0], 1);
    EXPECT_EQ(constructed.at(2), 3);

    Tensor<int, 2, 1, 2, 3> tensor;
    tensor[0] = 99;
    tensor.at(0, 0) = 10;
    tensor.at(1, 2) = 20;
    EXPECT_EQ(tensor[0], 99);
    EXPECT_EQ(tensor[6], 10);
    EXPECT_EQ(tensor[13], 20);
    EXPECT_EQ(tensor.at(0, 0), 10);
    EXPECT_EQ(tensor.at(1, 2), 20);

    const auto negated = -constructed;
    EXPECT_EQ(negated[0], -1);
    EXPECT_EQ(negated[2], -3);

    Tensor<int, 1, 0, 3> left{ 1, 2, 3 };
    Tensor<int, 1, 0, 3> right{ 4, 6, 8 };

    auto sum = left + right;
    EXPECT_EQ(sum[0], 5);
    EXPECT_EQ(sum[2], 11);

    auto difference = right - left;
    EXPECT_EQ(difference[0], 3);
    EXPECT_EQ(difference[2], 5);

    auto scaled_right = left * 3;
    EXPECT_EQ(scaled_right[0], 3);
    EXPECT_EQ(scaled_right[2], 9);

    auto scaled_left = 2 * left;
    EXPECT_EQ(scaled_left[0], 2);
    EXPECT_EQ(scaled_left[2], 6);

    auto divided = right / 2;
    EXPECT_EQ(divided[0], 2);
    EXPECT_EQ(divided[2], 4);

    left += Tensor<int, 1, 0, 3>{ 1, 1, 1 };
    EXPECT_EQ(left[0], 2);
    EXPECT_EQ(left[2], 4);

    left -= Tensor<int, 1, 0, 3>{ 2, 1, 0 };
    EXPECT_EQ(left[0], 0);
    EXPECT_EQ(left[2], 4);

    left *= 3;
    EXPECT_EQ(left[1], 6);
    EXPECT_EQ(left[2], 12);

    left /= 2;
    EXPECT_EQ(left[1], 3);
    EXPECT_EQ(left[2], 6);
}

void testArrayNDConstructionAccessAndFill()
{
    using toolbox::ArrayND;
    using toolbox::Vec;

    ArrayND<int, 2> empty;
    EXPECT_TRUE(empty.empty());
    EXPECT_EQ(empty.size(), std::size_t{ 0 });

    ArrayND<int, 2> array{ Vec<std::size_t, 2>{ 2, 3 }, 7 };
    EXPECT_FALSE(array.empty());
    EXPECT_EQ(array.size(), std::size_t{ 6 });
    EXPECT_EQ(array.shape()[0], std::size_t{ 2 });
    EXPECT_EQ(array.shape()[1], std::size_t{ 3 });
    EXPECT_EQ(array.strides()[0], std::size_t{ 3 });
    EXPECT_EQ(array.strides()[1], std::size_t{ 1 });

    EXPECT_EQ(array[0], 7);
    EXPECT_EQ(array.at(1, 2), 7);
    const Vec<int, 2> last_index{ 1, 2 };
    EXPECT_EQ(array(last_index), 7);

    array[0] = 10;
    array.at(last_index) = 20;
    array(0, 1) = 30;
    EXPECT_EQ(array[0], 10);
    EXPECT_EQ(array.at(1, 2), 20);
    EXPECT_EQ(array(0, 1), 30);

    const auto& const_array = array;
    EXPECT_EQ(const_array[0], 10);
    EXPECT_EQ(const_array.at(last_index), 20);
    EXPECT_EQ(const_array(0, 1), 30);
    EXPECT_EQ(const_array(last_index), 20);

    array.fill(5);
    EXPECT_EQ(array[0], 5);
    EXPECT_EQ(array[5], 5);

    EXPECT_THROWS(array.at(-1, 0), std::out_of_range);
    EXPECT_THROWS(array.at(2, 0), std::out_of_range);
}

void testArrayNDArithmeticAndErrors()
{
    using toolbox::ArrayND;
    using toolbox::Vec;

    ArrayND<int, 2> left{ Vec<std::size_t, 2>{ 2, 2 } };
    left(0, 0) = 1;
    left(0, 1) = 2;
    left(1, 0) = 3;
    left(1, 1) = 4;

    ArrayND<int, 2> right{ Vec<std::size_t, 2>{ 2, 2 } };
    right(0, 0) = 10;
    right(0, 1) = 20;
    right(1, 0) = 30;
    right(1, 1) = 40;

    const auto negated = -left;
    EXPECT_EQ(negated(0, 0), -1);
    EXPECT_EQ(negated(1, 1), -4);

    const auto sum = left + right;
    EXPECT_EQ(sum(0, 0), 11);
    EXPECT_EQ(sum(1, 1), 44);

    const auto difference = right - left;
    EXPECT_EQ(difference(0, 0), 9);
    EXPECT_EQ(difference(1, 1), 36);

    const auto scaled_right = left * 3;
    EXPECT_EQ(scaled_right(0, 0), 3);
    EXPECT_EQ(scaled_right(1, 1), 12);

    const auto scaled_left = 2 * left;
    EXPECT_EQ(scaled_left(0, 0), 2);
    EXPECT_EQ(scaled_left(1, 1), 8);

    const auto divided = right / 10;
    EXPECT_EQ(divided(0, 0), 1);
    EXPECT_EQ(divided(1, 1), 4);

    left += ArrayND<int, 2>{ Vec<std::size_t, 2>{ 2, 2 }, 1 };
    EXPECT_EQ(left(0, 0), 2);
    EXPECT_EQ(left(1, 1), 5);

    left -= ArrayND<int, 2>{ Vec<std::size_t, 2>{ 2, 2 }, 2 };
    EXPECT_EQ(left(0, 0), 0);
    EXPECT_EQ(left(1, 1), 3);

    left *= 3;
    EXPECT_EQ(left(0, 1), 3);
    EXPECT_EQ(left(1, 1), 9);

    left /= 3;
    EXPECT_EQ(left(0, 1), 1);
    EXPECT_EQ(left(1, 1), 3);

    const ArrayND<int, 2> mismatched{ Vec<std::size_t, 2>{ 2, 3 } };
    EXPECT_THROWS(left += mismatched, std::invalid_argument);
    EXPECT_THROWS(left -= mismatched, std::invalid_argument);
    EXPECT_THROWS(left + mismatched, std::invalid_argument);
    EXPECT_THROWS(left - mismatched, std::invalid_argument);
}
}

int main()
{
    runTest("Vec construction and access", testVecConstructionAndAccess);
    runTest("Vec arithmetic", testVecArithmetic);
    runTest("Vec floating-point helpers", testVecFloatingPointHelpers);
    runTest("Point", testPoint);
    runTest("PointVecOps", testPointVecOps);
    runTest("FieldTypes", testFieldTypes);
    runTest("ScalarField index bounds", testScalarFieldIndexBounds);
    runTest("FluidGrid boundary cell mask", testFluidGridInitializesBoundarySolids);
    runTest("World update near bottom boundary", testWorldUpdateNearBottomBoundary);
    runTest("World update seeded lower half fluid", testWorldUpdateSeededLowerHalfFluid);
    runTest("Tensor construction, access, and arithmetic", testTensorConstructionAccessAndArithmetic);
    runTest("ArrayND construction, access, and fill", testArrayNDConstructionAccessAndFill);
    runTest("ArrayND arithmetic and errors", testArrayNDArithmeticAndErrors);

    if (failed_expectations == 0) {
        std::cout << "All tests passed.\n";
        return 0;
    }

    std::cerr << failed_expectations << " expectation(s) failed.\n";
    return 1;
}
