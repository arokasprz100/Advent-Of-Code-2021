#pragma once

struct DiveCommand {
    std::string direction{};
    int value{};
};


class Submarine {
public:
    virtual ~Submarine() = default;
    virtual void forward(int how_much) = 0;
    virtual void up(int how_much) = 0;
    virtual void down(int how_much) = 0;

    [[nodiscard]] int get_depth() const { return current_depth; }
    [[nodiscard]] int get_horizontal_position() const { return current_horizontal_position; }

protected:

    void increase_depth(int how_much) {
        current_depth += how_much;
    }

    void decrease_depth(int how_much) {
        current_depth -= how_much;
    }

    void increase_horizontal_position(int how_much) {
        current_horizontal_position += how_much;
    }

private:
    int current_depth{};
    int current_horizontal_position{};
};


class SimpleSubmarine : public Submarine {
public:
    void forward(int how_much) override {
        increase_horizontal_position(how_much);
    }

    void up(int how_much) override {
        decrease_depth(how_much);
    }

    void down(int how_much) override {
        increase_depth(how_much);
    }

};


class SubmarineWithAim : public Submarine {
public:

    void forward(int how_much) override {
        increase_horizontal_position(how_much);
        increase_depth(how_much * current_aim);
    }

    void up(int how_much) override {
        current_aim -= how_much;
    }

    void down(int how_much) override {
        current_aim += how_much;
    }

private:
    int current_aim{};
};
