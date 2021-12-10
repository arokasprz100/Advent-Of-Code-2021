#pragma once

class Map {
public:

    Map(unsigned max_x_coordinate, unsigned max_y_coordinate) :
            occupation_counts_per_point(max_x_coordinate, std::vector<unsigned>(max_y_coordinate, 0)) {}

    void increase_occupation_count(const Point& point) {
        ++occupation_counts_per_point.at(point.first).at(point.second);
    }

    [[nodiscard]]
    unsigned count_points_where_lines_overlap() const {
        unsigned number_of_points{0};
        for(const auto& column : occupation_counts_per_point) {
            for (unsigned int count: column) {
                number_of_points = count > 1 ? number_of_points + 1 : number_of_points;
            }
        }
        return number_of_points;
    }

private:
    std::vector<std::vector<unsigned>> occupation_counts_per_point{};
};