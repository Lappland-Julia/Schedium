#include "Validator.h"

#include <stdexcept>

std::string Validator::get_validating_power() const {
    return this->VALIDATING_TYPES[this->VALIDATING_TYPES.size() - this->validating_power];
}

void Validator::set_validating_power(const std::string &new_validating_power) {
    int parsed_value = this->VALIDATING_TYPES.size();
    for (auto power: this->VALIDATING_TYPES) {
        if (new_validating_power == power) {
            this->validating_power = parsed_value;
            return;
        }
        else {
            parsed_value--;
        }
    }
    if (parsed_value == 0) {
        throw std::runtime_error(new_validating_power +" is not a valid type of validator. Check Validator.get_validating_types()");
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
