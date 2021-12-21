#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <Utils.h>

class Image {
public:

    Image(std::vector<bool> image_data, unsigned width, unsigned height)
            : image_data_row_major{std::move(image_data)}, image_width{width}, image_height{height} {
        if(width * height != image_data_row_major.size()) {
            throw std::runtime_error{"Image data size does not match given width and height"};
        }
    }

    [[nodiscard]]
    inline unsigned width() const {
        return image_width;
    }

    [[nodiscard]]
    inline unsigned height() const {
        return image_height;
    }

    inline void set(unsigned x, unsigned y, bool value) {
        image_data_row_major.at(y * image_width + x) = value;
    }

    [[nodiscard]]
    unsigned get_neighbours_value(unsigned x, unsigned y, bool out_of_bounds_value = false) const {
        unsigned neighbours_value = 0;
        unsigned position = 8;
        for(int y_mod = -1; y_mod <= 1; ++y_mod) {
            for(int x_mod = -1; x_mod <= 1; ++x_mod) {
                int x_neighbour = static_cast<int>(x) + x_mod;
                int y_neighbour = static_cast<int>(y) + y_mod;
                bool value = is_out_of_bounds(x_neighbour, y_neighbour)
                        ? out_of_bounds_value
                        : image_data_row_major.at(y_neighbour * image_width + x_neighbour);
                neighbours_value |= (static_cast<unsigned>(value) << position);
                --position;
            }
        }
        return neighbours_value;
    }

    [[nodiscard]]
    inline unsigned active_pixel_count() const {
        return std::count(std::begin(image_data_row_major), std::end(image_data_row_major), true);
    }

    void enlarge(unsigned width_enlargement, unsigned height_enlargement, bool default_value = false) {
        unsigned new_width = image_width + width_enlargement * 2;
        unsigned new_height = image_height + height_enlargement * 2;
        std::vector<bool> new_image_data(new_height * new_width, default_value);
        for(unsigned y = 0; y < image_height; ++y) {
            for(unsigned x = 0; x < image_width; ++x) {
                new_image_data.at((y + height_enlargement) * new_width + x + width_enlargement) = image_data_row_major.at(y * image_width + x);
            }
        }
        image_data_row_major = new_image_data;
        image_width = new_width;
        image_height = new_height;
    }

private:
    std::vector<bool> image_data_row_major{};
    unsigned image_width{};
    unsigned image_height{};

    [[nodiscard]]
    inline bool is_out_of_bounds(int x, int y) const {
        return (x < 0 || x > static_cast<int>(image_width) - 1 || y < 0 || y > static_cast<int>(image_height) - 1);
    }
};

using ImageEnhancementAlgorithm = std::vector<bool>;

ImageEnhancementAlgorithm read_image_enhancement_algorithm(std::ifstream& file) {
    if(std::string input_line{}; std::getline(file, input_line)) {
        input_line = utils::trim(input_line);
        ImageEnhancementAlgorithm image_enhancement_algorithm{};
        for(const char character : input_line) {
            image_enhancement_algorithm.emplace_back(character == '#');
        }
        return image_enhancement_algorithm;
    }
    throw std::runtime_error{"Could not read the algorithm"};
}

Image read_infinite_image(std::ifstream& file) {
    unsigned image_width = 0;
    unsigned image_height = 0;
    std::vector<bool> image_data_row_major{};
    for(std::string input_line{}; std::getline(file, input_line); ) {
        input_line = utils::trim(input_line);
        if(input_line.empty()) {
            continue;
        }
        image_width = 0;
        std::stringstream input_line_string{input_line};
        for(char input_character{}; input_line_string >> input_character; ) {
            image_data_row_major.emplace_back(input_character == '#');
            ++image_width;
        }
        ++image_height;
    }
    return Image{image_data_row_major, image_width, image_height};
}

std::pair<ImageEnhancementAlgorithm, Image> read_puzzle_input(const std::string& file_name) {
    std::ifstream file{file_name};
    if(!file.is_open()) {
        throw std::runtime_error{"Could not open file " + file_name};
    }
    const auto image_enhancement_algorithm = read_image_enhancement_algorithm(file);
    const auto image = read_infinite_image(file);
    return { image_enhancement_algorithm, image };
}

Image enhance_image(const ImageEnhancementAlgorithm& algorithm, const Image& image, bool out_of_bounds_value) {
    Image new_image = image;
    for(unsigned y = 0; y < image.height(); ++y) {
        for(unsigned x = 0; x < image.width(); ++x) {
            unsigned new_value_index = image.get_neighbours_value(x, y, out_of_bounds_value);
            bool new_value = algorithm.at(new_value_index);
            new_image.set(x, y, new_value);
        }
    }
    return new_image;
}

unsigned enhance_image_and_count_lit_pixels(const ImageEnhancementAlgorithm& algorithm, Image image, unsigned number_of_steps) {
    image.enlarge(number_of_steps, number_of_steps);
    bool out_of_bounds_value = false;
    bool all_neighbours_off_value = algorithm.front();
    bool all_neighbours_on_value = algorithm.back();
    for(unsigned step = 0; step < number_of_steps; ++step) {
        image = enhance_image(algorithm, image, out_of_bounds_value);
        out_of_bounds_value = out_of_bounds_value ? all_neighbours_on_value : all_neighbours_off_value;
    }
    return image.active_pixel_count();
}

int main() {
    const auto[image_enhancement_algorithm, infinite_image] = read_puzzle_input("input.txt");
    std::cout << "Part 1: " << enhance_image_and_count_lit_pixels(image_enhancement_algorithm, infinite_image, 2) << std::endl;
    std::cout << "Part 2: " << enhance_image_and_count_lit_pixels(image_enhancement_algorithm, infinite_image, 50) << std::endl;
    return 0;
}