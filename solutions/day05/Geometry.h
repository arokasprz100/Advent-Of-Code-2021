#pragma once

using Point = std::pair<unsigned, unsigned>;

class Line{
public:

    Line(Point start, Point end) : start{std::move(start)}, end{std::move(end)} {}

    [[nodiscard]]
    bool is_horizontal() const {
        return start.first == end.first;
    }

    [[nodiscard]]
    bool is_vertical() const {
        return start.second == end.second;
    }

    [[nodiscard]]
    bool is_diagonal() const {
        unsigned x_difference = get_coordinates_difference(start.first, end.first);
        unsigned y_difference = get_coordinates_difference(start.second, end.second);
        return x_difference == y_difference;
    }

    [[nodiscard]]
    std::vector<Point> get_points() const {
        if(!is_horizontal() && !is_diagonal() && !is_vertical()) {
            throw std::runtime_error{"Unsupported line type"};
        }

        int x_multiplier = get_direction_multiplier(start.first, end.first);
        int y_multiplier = get_direction_multiplier(start.second, end.second);
        unsigned x_difference = get_coordinates_difference(start.first, end.first);
        unsigned y_difference = get_coordinates_difference(start.second, end.second);
        unsigned difference = std::max(x_difference, y_difference);

        std::vector<Point> points{};
        for(unsigned i = 0; i <= difference; ++i) {
            points.emplace_back(start.first + i * x_multiplier, start.second + i * y_multiplier);
        }
        return points;
    }

private:
    Point start{};
    Point end{};

    [[nodiscard]]
    static int get_direction_multiplier(unsigned line_start_coord, unsigned line_end_coord) {
        if(line_start_coord == line_end_coord) return 0;
        else if(line_start_coord > line_end_coord) return -1;
        else return 1;
    }

    [[nodiscard]]
    static unsigned get_coordinates_difference(unsigned line_start_coord, unsigned line_end_coord) {
        return static_cast<unsigned>(std::abs(static_cast<int>(line_start_coord) - static_cast<int>(line_end_coord)));
    }
};