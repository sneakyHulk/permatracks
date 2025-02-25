#include <MagneticFluxDensityData.h>
#include <common_istream.h>
#include <common_output.h>
#include <matplotlibcpp17/mplot3d.h>
#include <matplotlibcpp17/patches.h>
#include <matplotlibcpp17/pyplot.h>

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
#include <filesystem>
#include <fstream>
#include <generator>
#include <ranges>

std::generator<std::tuple<double, double, double>> file_mag_data_generator(std::istream&& stream) {
	for (double x, y, z; stream >> x >> common::chlit(',') >> y >> common::chlit(',') >> z;) {
		co_yield std::make_tuple(x, y, z);
	}
}

// see paper: Qingde Li and J. G. Griffiths, "Least squares ellipsoid specific fitting," Geometric Modeling and Processing, 2004. Proceedings, Beijing, China, 2004, pp. 335-340, doi: 10.1109/GMAP.2004.1290055.
Eigen::Vector<double, 10> least_squares_ellipsoid_specific_fitting(std::ranges::viewable_range auto points) {
	// design matrix D:
	Eigen::Matrix<double, 10, Eigen::Dynamic> D;
	for (auto cols = 0; auto const [x, y, z] : points) {
		D.conservativeResize(Eigen::NoChange, ++cols);

		D(0, cols - 1) = x * x;
		D(1, cols - 1) = y * y;
		D(2, cols - 1) = z * z;
		D(3, cols - 1) = 2.0 * y * z;
		D(4, cols - 1) = 2.0 * x * z;
		D(5, cols - 1) = 2.0 * x * y;
		D(6, cols - 1) = 2.0 * x;
		D(7, cols - 1) = 2.0 * y;
		D(8, cols - 1) = 2.0 * z;
		D(9, cols - 1) = 1.0;
	}

	Eigen::Matrix<double, 10, 10> S = D * D.transpose();

	Eigen::Matrix<double, 6, 6> const S11 = S.block<6, 6>(0, 0);
	Eigen::Matrix<double, 6, 4> const S12 = S.block<6, 4>(0, 6);
	Eigen::Matrix<double, 4, 6> const S21 = S.block<4, 6>(6, 0);
	Eigen::Matrix<double, 4, 4> const S22 = S.block<4, 4>(6, 6);

	// constraint matrix C:
	Eigen::Matrix<double, 6, 6> C = decltype(C)::Zero();
	C.block<3, 3>(0, 0) = Eigen::Matrix<double, 3, 3>::Ones();
	C.block<3, 3>(0, 0).diagonal() = -Eigen::Vector<double, 3>::Ones();
	C.block<3, 3>(3, 3).diagonal() = -4.0 * Eigen::Vector<double, 3>::Ones();

	Eigen::Matrix<double, 6, 6> const A = S11 - S12 * S22.inverse() * S21;  // if S22 is almost singular S22^-1 can be replaced with generalized inverse (.completeOrthogonalDecomposition().pseudoInverse())
	Eigen::Matrix<double, 6, 6> CA = C.inverse() * A;

	Eigen::EigenSolver<decltype(CA)> const solver(CA);

	// Find the largest eigenvalue(the only positive eigenvalue)
	Eigen::Index index_largest_eigenvalue;
	solver.eigenvalues().real().maxCoeff(&index_largest_eigenvalue);

	Eigen::Vector<double, 6> const v1 = solver.eigenvectors().col(index_largest_eigenvalue).real();
	Eigen::Vector<double, 4> const v2 = S22.inverse() * S21 * v1;

	// ellipsoid fitting v
	Eigen::Vector<double, 10> v;
	v << v1, -v2;

	return v;
}

std::tuple<Eigen::Vector<double, 3>, Eigen::Matrix<double, 3, 3>> compute_offsets(Eigen::Vector<double, 10> const& v, double norm_of_magnetic_field) {
	// An ellipsoid can be described by the general fom of quadric surfaces:
	// Ax² + By² + Cz² + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz + J = 0

	// The equation of the general quadric can also be put into matrix form:
	//                      ┌ A D E G ┐          ┌ x ┐
	// xT*Q*x = 0, with Q = │ D B F H │, and x = │ y │
	//                      │ E F C I │          │ z │
	//                      └ G H I J ┘          └ 1 ┘

	//              ┌ A D E ┐         ┌ E ┐
	// Defining Q = │ D B F │ and U = │ F │,
	//              └ E F C ┘         └ C ┘

	Eigen::Matrix<double, 3, 3> Q;
	Q << v(0), v(5), v(4), v(5), v(1), v(3), v(4), v(3), v(2);
	Eigen::Vector<double, 3> U;
	U << v(6), v(7), v(8);

	// the center of the ellipsoid can be calculated as follows:
	Eigen::Vector<double, 3> const B = -(Q.inverse() * U);

	Eigen::EigenSolver<Eigen::Matrix<double, 3, 3>> const solver(Q);

	// The ellipsoid's semi-axis a = sqrt((BT*Q*B-J) / λ1), b = sqrt((BT*Q*B-J) / λ2) and c = sqrt((BT*Q*B-J) / λ3), with λ eigenvalues of Q.
	double abc = std::sqrt(B.transpose() * Q * B - v(9));
	Eigen::DiagonalMatrix<double, 3> D = solver.eigenvalues().real().cwiseSqrt().asDiagonal() * (1 / abc);

	// The normalized eigenvectors describe the direction of the 3 principal axis of the ellipsoid.
	Eigen::Matrix<double, 3, 3> SQ = solver.eigenvectors().real() * D * solver.eigenvectors().real().transpose();

	return {B, SQ * norm_of_magnetic_field};
}

int main() {
	Eigen::Vector<double, 10> const fitting = least_squares_ellipsoid_specific_fitting(file_mag_data_generator(std::ifstream(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "mag_data_2025Feb19_9h30min1s")));

	auto const [B, A_1] = compute_offsets(fitting, 47.703);

	pybind11::scoped_interpreter guard{};
	auto plt = matplotlibcpp17::pyplot::import();

	auto fig = plt.figure(Args(), Kwargs("figsize"_a = py::make_tuple(12.f, 4.8f)));
	auto fig2 = plt.figure(Args(), Kwargs("figsize"_a = py::make_tuple(12.f, 4.8f)));
	{
		std::ifstream file(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "mag_data_2025Feb19_9h30min1s");
		Eigen::Matrix<double, Eigen::Dynamic, 3> data;
		unsigned int row = 0;
		for (double x, y, z; file >> x >> common::chlit(',') >> y >> common::chlit(',') >> z; ++row) {
			data.conservativeResize(row + 1, Eigen::NoChange);

			data(row, 0) = x;
			data(row, 1) = y;
			data(row, 2) = z;

			data.row(row) = A_1 * (data.row(row).transpose() - B);
		}

		std::vector<double> const x(data.col(0).array().begin(), data.col(0).array().end());
		std::vector<double> const y(data.col(1).array().begin(), data.col(1).array().end());
		std::vector<double> const z(data.col(2).array().begin(), data.col(2).array().end());

		matplotlibcpp17::mplot3d::import();
		auto ax = fig.add_subplot(Args(121), Kwargs("projection"_a = "3d"));

		ax.scatter(Args(x, y, z, 'z', 1.5f));
		ax.set_aspect(Args("equal"));

		auto ax2 = fig2.add_subplot(Args(121));

		ax2.scatter(Args(x, y, 1.5f, "green"));
		ax2.scatter(Args(y, z, 1.5f, "blue"));
		ax2.scatter(Args(z, x, 1.5f, "red"));

		matplotlibcpp17::patches::Circle circle(Args(py::make_tuple(0, 0)), Kwargs("color"_a = "black", "fill"_a = false, "radius"_a = 50));
		ax2.add_patch(Args(circle.unwrap()));

		ax2.set_aspect(Args("equal"));
	}
	{
		std::ifstream file(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "mag_data_2025Feb19_9h30min1s");
		std::vector<double> x;
		std::vector<double> y;
		std::vector<double> z;
		for (double x_e, y_e, z_e; file >> x_e >> common::chlit(',') >> y_e >> common::chlit(',') >> z_e;) {
			x.push_back(x_e);
			y.push_back(y_e);
			z.push_back(z_e);
		}

		matplotlibcpp17::mplot3d::import();
		auto ax = fig.add_subplot(Args(122), Kwargs("projection"_a = "3d"));

		ax.scatter(Args(x, y, z, 'z', 1.5f));
		ax.set_aspect(Args("equal"));

		auto ax2 = fig2.add_subplot(Args(122));

		ax2.scatter(Args(x, y, 1.5f, "green"));
		ax2.scatter(Args(y, z, 1.5f, "blue"));
		ax2.scatter(Args(z, x, 1.5f, "red"));

		matplotlibcpp17::patches::Circle circle(Args(py::make_tuple(0, 0)), Kwargs("color"_a = "black", "fill"_a = false, "radius"_a = 50));
		ax2.add_patch(Args(circle.unwrap()));

		ax2.set_aspect(Args("equal"));
	}

	// 	std::ofstream outfile(std::filesystem::path(CMAKE_SOURCE_DIR) / "result" / "mag_data_2025Feb19_9h30min1s2");
	// for (auto const& [e_x, e_y, e_z] : std::ranges::views::zip(x, y, z)) {
	// 	outfile << e_x << '\t' << e_y << '\t' << e_z << std::endl;
	// }

	plt.show();

	return EXIT_SUCCESS;
}