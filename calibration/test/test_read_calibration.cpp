#include <csv.hpp>
#include <filesystem>
#include <ranges>
#include <set>
#include <generator>
#include <common_output.h>

std::generator<std::tuple<std::string, std::string, std::string> > get_x_y_z_names(std::vector<std::string> col_names) {
    std::set<std::string> cols(col_names.begin(), col_names.end());

    for (auto i = 0; cols.contains(common::stringprint('x', i)) && cols.contains(common::stringprint('y', i)) && cols.contains(common::stringprint('z', i)); ++i) {
        co_yield {common::stringprint('x', i), common::stringprint('y', i), common::stringprint('z', i)};
    }
}

int main() {
    csv::CSVReader reader(
        (std::filesystem::path(CMAKE_SOURCE_DIR) / "result" /
         "mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h26min51s_calibration_x.txt").string());

    for (const auto &row: reader | std::ranges::views::drop(1)) {
        for (const auto [x, y, z]: get_x_y_z_names(row.get_col_names())) {
            std::cout << row[x].get<double>() << std::endl;
        }
    }
}
