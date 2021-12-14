#include <iostream>
#include <fstream>
#include <unordered_set>

#include <Utils.h>

using AdjacencyMultimap = std::unordered_multimap<std::string, std::string>;

std::vector<std::pair<std::string, std::string>> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    std::vector<std::pair<std::string, std::string>>  puzzle_input{};
    for(std::string input_line{}; std::getline(file, input_line); ) {
        input_line = utils::trim(input_line);
        if(!input_line.empty()) {
            const auto parts = utils::split_by_delimiter(input_line, "-");
            puzzle_input.emplace_back(utils::trim(parts.at(0)), utils::trim(parts.at(1)));
        }
    }
    return puzzle_input;
}

AdjacencyMultimap construct_adjacency_multimap(const std::vector<std::pair<std::string, std::string>>& connections) {
    AdjacencyMultimap adjacency_multimap{};
    for(const auto& connection: connections) {
        adjacency_multimap.insert({ connection.first, connection.second });
        adjacency_multimap.insert({ connection.second, connection.first });
    }
    return adjacency_multimap;
}

bool is_cave_large(const std::string& cave_name) {
    return std::isupper(static_cast<unsigned char>(cave_name.at(0)));
}

bool was_small_cave_visited(const std::string& cave_name, std::unordered_map<std::string, unsigned>& small_caves_visits_counter) {
    return small_caves_visits_counter[cave_name] > 0;
}

bool has_any_small_cave_been_visited_more_than_once(const std::unordered_map<std::string, unsigned>& small_caves_visits_counter) {
    const auto predicate = [](const auto& cave_entry) {
        return cave_entry.second > 1;
    };
    return std::any_of(std::begin(small_caves_visits_counter), std::end(small_caves_visits_counter), predicate);
}


class CavePossibleToVisitAccordingToPartOneRulesPredicate {
public:
    static bool check(const std::string& cave_name, std::unordered_map<std::string, unsigned>& small_caves_visits_counter) {
        return is_cave_large(cave_name) || !was_small_cave_visited(cave_name, small_caves_visits_counter);
    }
};


class CavePossibleToVisitAccordingToPartTwoRulesPredicate {
public:
    static bool check(const std::string& cave_name, std::unordered_map<std::string, unsigned>& small_caves_visits_counter) {
        if(is_cave_large(cave_name)) {
            return true;
        }
        if(was_small_cave_visited(cave_name, small_caves_visits_counter)) {
            return !(cave_name == "start" || cave_name == "end") &&
                   !has_any_small_cave_been_visited_more_than_once(small_caves_visits_counter);
        }
        return true;
    }
};


template<typename CavePossibleToVisitPredicate>
class PathInCavesFinder {
public:

    explicit PathInCavesFinder(AdjacencyMultimap graph) : graph{std::move(graph)} {}

    unsigned find_number_of_paths_from_start_to_end(const std::string& start, const std::string& end) {
        small_caves_visits_counter.clear();
        return process_single_cave(start, end);
    }

private:
    AdjacencyMultimap graph{};
    std::unordered_map<std::string, unsigned> small_caves_visits_counter{};

    unsigned process_single_cave(const std::string& cave, const std::string& end) {
        if(is_cave_large(cave)) {
            return visit_adjacent_caves(cave, end);
        }
        else {
            ++small_caves_visits_counter[cave];
            unsigned number_of_paths = visit_adjacent_caves(cave, end);
            --small_caves_visits_counter[cave];
            return number_of_paths;
        }
    }

    unsigned visit_adjacent_caves(const std::string& vertex, const std::string& end) {
        unsigned number_of_paths{0};
        const auto adjacent_caves_range = graph.equal_range(vertex);
        for (auto i = adjacent_caves_range.first; i != adjacent_caves_range.second; ++i) {
            const std::string& adjacent_cave = i->second;
            if(adjacent_cave == end) {
                ++number_of_paths;
                continue;
            }
            if(CavePossibleToVisitPredicate::check(adjacent_cave, small_caves_visits_counter)) {
                number_of_paths += process_single_cave(adjacent_cave, end);
            }
        }
        return number_of_paths;
    }
};


unsigned solve_part_one(const std::vector<std::pair<std::string, std::string>>& paths) {
    const auto adjacency_multimap = construct_adjacency_multimap(paths);
    PathInCavesFinder<CavePossibleToVisitAccordingToPartOneRulesPredicate> finder{adjacency_multimap};
    return finder.find_number_of_paths_from_start_to_end("start", "end");
}

unsigned solve_part_two(const std::vector<std::pair<std::string, std::string>>& paths) {
    const auto adjacency_multimap = construct_adjacency_multimap(paths);
    PathInCavesFinder<CavePossibleToVisitAccordingToPartTwoRulesPredicate> finder{adjacency_multimap};
    return finder.find_number_of_paths_from_start_to_end("start", "end");
}

int main() {
    const auto puzzle_input = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << solve_part_one(puzzle_input) << std::endl;
    std::cout << "Part 2: " << solve_part_two(puzzle_input) << std::endl;
    return 0;
}