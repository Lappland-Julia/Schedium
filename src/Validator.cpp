#include "Validator.h"

#include <stdexcept>

std::string Validator::get_validating_level() const {
    switch (this->validating_level) {
        case 1:
            return "EASY";
            break;
        case 2:
            return "MEDIUM";
            break;
        case 3:
            return "HARD";
            break;
        default:
            return "UNKNOWN";
    }
}

void Validator::set_validating_level(const std::string &new_validating_level) {
    if (new_validating_level == "EASY") {
        this->validating_level = 1;
    }
    else if (new_validating_level == "MEDIUM") {
        this->validating_level = 2;
    }
    else if (new_validating_level == "HARD") {
        this->validating_level = 3;
    }
    else {
        throw std::invalid_argument("Invalid validating level");
    }
}

void Validator::add_manager(const ScheduleManager &Manager) {
    this->managers.push_back(Manager);
}

void Validator::add_managers(const std::vector<ScheduleManager> &Managers) {
    for (ScheduleManager manager: Managers) {
        this->add_manager(manager);
    }
}

Validator::Validator() {
    this->managers = std::vector<ScheduleManager>{};
}

Validator::Validator(const ScheduleManager &Manager) {
    this->add_manager(Manager);
}

Validator::Validator(const std::vector<ScheduleManager> &Managers) {
    this->add_managers(Managers);
}
