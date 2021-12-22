#include <iostream>
#include <fstream>
#include <vector>
#include <regex>
#include <optional>
#include <numeric>
#include <Utils.h>

using Range = std::pair<int, int>;

struct Cuboid {
    Range x_range{};
    Range y_range{};
    Range z_range{};
};

struct RebootStep {
    Cuboid ranges{};
    bool value{};
};

RebootStep parse_reboot_step(const std::string& input_line, const std::regex& reboot_step_regex) {
    std::smatch matches{};
    if(std::regex_search(input_line, matches, reboot_step_regex)) {
        bool value = (matches[1].str() == "on");
        Range x_range{ std::stoi(matches[2].str()), std::stoi(matches[3].str()) };
        Range y_range{ std::stoi(matches[4].str()), std::stoi(matches[5].str()) };
        Range z_range{ std::stoi(matches[6].str()), std::stoi(matches[7].str()) };
        return RebootStep{Cuboid{x_range, y_range, z_range}, value};
    }
    throw std::runtime_error{"Input line does not match reboot step regex"};
}

std::vector<RebootStep> read_puzzle_input(const std::string& file_name) {
    static const std::regex reboot_step_regex{"(on|off) x=(-?[0-9]+)..(-?[0-9]+),y=(-?[0-9]+)..(-?[0-9]+),z=(-?[0-9]+)..(-?[0-9]+)"};
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::vector<RebootStep> reboot_steps{};
    for(std::string input_line{}; std::getline(file, input_line); ) {
        input_line = utils::trim(input_line);
        if(!input_line.empty()) {
            auto reboot_step = parse_reboot_step(input_line, reboot_step_regex);
            reboot_steps.push_back(reboot_step);
        }
    }
    return reboot_steps;
}

std::optional<Cuboid> calculate_intersection(const Cuboid& cuboid_a, const Cuboid& cuboid_b) {
    int x_start = std::max(cuboid_a.x_range.first, cuboid_b.x_range.first);
    int x_end = std::min(cuboid_a.x_range.second, cuboid_b.x_range.second);
    if(x_start > x_end) {
        return std::nullopt;
    }
    int y_start = std::max(cuboid_a.y_range.first, cuboid_b.y_range.first);
    int y_end = std::min(cuboid_a.y_range.second, cuboid_b.y_range.second);
    if(y_start > y_end) {
        return std::nullopt;
    }
    int z_start = std::max(cuboid_a.z_range.first, cuboid_b.z_range.first);
    int z_end = std::min(cuboid_a.z_range.second, cuboid_b.z_range.second);
    if(z_start > z_end) {
        return std::nullopt;
    }
    return Cuboid{{x_start, x_end}, {y_start, y_end}, {z_start, z_end}};
}

std::vector<Range> generate_split_ranges_along_x(const Cuboid& to_split, const Cuboid& intersection) {
    std::vector<Range> x_ranges{};
    if(to_split.x_range.first <= intersection.x_range.first - 1) {
        x_ranges.emplace_back(to_split.x_range.first, intersection.x_range.first - 1);
    }
    if(intersection.x_range.first <= intersection.x_range.second) {
        x_ranges.emplace_back(intersection.x_range.first, intersection.x_range.second);
    }
    if(intersection.x_range.second + 1 <= to_split.x_range.second) {
        x_ranges.emplace_back(intersection.x_range.second + 1, to_split.x_range.second);
    }
    return x_ranges;
}

std::vector<Range> generate_split_ranges_along_y(const Cuboid& to_split, const Cuboid& intersection) {
    std::vector<Range> y_ranges{};
    if(to_split.y_range.first <= intersection.y_range.first - 1) {
        y_ranges.emplace_back(to_split.y_range.first, intersection.y_range.first - 1);
    }
    if(intersection.y_range.first <= intersection.y_range.second) {
        y_ranges.emplace_back(intersection.y_range.first, intersection.y_range.second);
    }
    if(intersection.y_range.second + 1 <= to_split.y_range.second) {
        y_ranges.emplace_back(intersection.y_range.second + 1, to_split.y_range.second);
    }
    return y_ranges;
}

std::vector<Range> generate_split_ranges_along_z(const Cuboid& to_split, const Cuboid& intersection) {
    std::vector<Range> z_ranges{};
    if(to_split.z_range.first <= intersection.z_range.first - 1) {
        z_ranges.emplace_back(to_split.z_range.first, intersection.z_range.first - 1);
    }
    if(intersection.z_range.first <= intersection.z_range.second) {
        z_ranges.emplace_back(intersection.z_range.first, intersection.z_range.second);
    }
    if(intersection.z_range.second + 1 <= to_split.z_range.second) {
        z_ranges.emplace_back(intersection.z_range.second + 1, to_split.z_range.second);
    }
    return z_ranges;
}

std::vector<Cuboid> split_on_intersection(const Cuboid& to_split, const Cuboid& intersection) {
    std::vector<Range> x_ranges = generate_split_ranges_along_x(to_split, intersection);
    std::vector<Range> y_ranges = generate_split_ranges_along_y(to_split, intersection);
    std::vector<Range> z_ranges = generate_split_ranges_along_z(to_split, intersection);
    std::vector<Cuboid> cuboids{};
    for(const auto& x_range: x_ranges) {
        for(const auto& y_range: y_ranges) {
            for(const auto& z_range: z_ranges) {
                if(x_range.first == intersection.x_range.first &&
                   y_range.first == intersection.y_range.first &&
                   z_range.first == intersection.z_range.first)  {
                    continue; // do not include the intersection itself
                }
                cuboids.emplace_back(Cuboid{x_range, y_range, z_range});
            }
        }
    }
    return cuboids;
}

long long calculate_volume(const Cuboid& cuboid) {
    return (std::llabs(cuboid.x_range.second - cuboid.x_range.first) + 1) *
           (std::llabs(cuboid.y_range.second - cuboid.y_range.first) + 1) *
           (std::llabs(cuboid.z_range.second - cuboid.z_range.first) + 1);
}

long long accumulate_cuboid_volumes(const std::vector<Cuboid>& cuboids) {
    const auto volume_accumulator = [](long long previous_volume, const Cuboid& cuboid) {
        return previous_volume + calculate_volume(cuboid);
    };
    return std::accumulate(std::begin(cuboids), std::end(cuboids), 0LL, volume_accumulator);
}

std::vector<Cuboid> execute_reboot_procedure_step(const RebootStep& reboot_step, const std::vector<Cuboid>& active_cuboids) {
    std::vector<Cuboid> cuboids_active_after_step{};
    for(const auto& active_cuboid: active_cuboids) {
        if(auto cuboids_intersection = calculate_intersection(reboot_step.ranges, active_cuboid); cuboids_intersection.has_value()) {
            const auto new_cuboids = split_on_intersection(active_cuboid, cuboids_intersection.value());
            cuboids_active_after_step.insert(std::end(cuboids_active_after_step), std::begin(new_cuboids), std::end(new_cuboids));
        }
        else {
            cuboids_active_after_step.push_back(active_cuboid);
        }
    }
    if(reboot_step.value) {
        cuboids_active_after_step.push_back(reboot_step.ranges);
    }
    return cuboids_active_after_step;
}

long long execute_reboot_procedure(const std::vector<RebootStep>& reboot_steps) {
    std::vector<Cuboid> active_cuboids{};
    for(const auto& reboot_step: reboot_steps) {
        active_cuboids = execute_reboot_procedure_step(reboot_step, active_cuboids);
    }
    return accumulate_cuboid_volumes(active_cuboids);
}

std::optional<RebootStep> transform_reboot_step_into_init_step(const RebootStep& reboot_step, int lower_bound = -50, int upper_bound = 50) {
    auto[x1, x2] = reboot_step.ranges.x_range;
    auto[y1, y2] = reboot_step.ranges.y_range;
    auto[z1, z2] = reboot_step.ranges.z_range;
    if((x1 < lower_bound && x2 < lower_bound) || (x1 > upper_bound && x2 > upper_bound)) return std::nullopt;
    if((y1 < lower_bound && y2 < lower_bound) || (y1 > upper_bound && y2 > upper_bound)) return std::nullopt;
    if((z1 < lower_bound && z2 < lower_bound) || (z1 > upper_bound && z2 > upper_bound)) return std::nullopt;
    Range new_x_range = { std::clamp(x1, lower_bound, upper_bound), std::clamp(x2, lower_bound, upper_bound) };
    Range new_y_range = { std::clamp(y1, lower_bound, upper_bound), std::clamp(y2, lower_bound, upper_bound) };
    Range new_z_range = { std::clamp(z1, lower_bound, upper_bound), std::clamp(z2, lower_bound, upper_bound) };
    return RebootStep{{new_x_range, new_y_range, new_z_range}, reboot_step.value};
}

std::vector<RebootStep> create_initialization_procedure_steps(const std::vector<RebootStep>& reboot_steps) {
    std::vector<RebootStep> init_procedure_steps{};
    for(const auto& reboot_step: reboot_steps) {
        if(auto init_step = transform_reboot_step_into_init_step(reboot_step); init_step.has_value()) {
            init_procedure_steps.push_back(init_step.value());
        }
    }
    return init_procedure_steps;
}

long long solve_part_one(const std::vector<RebootStep>& reboot_steps) {
    const auto initialization_procedure_steps = create_initialization_procedure_steps(reboot_steps);
    return execute_reboot_procedure(initialization_procedure_steps);
}

long long solve_part_two(const std::vector<RebootStep>& reboot_steps) {
    return execute_reboot_procedure(reboot_steps);
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}