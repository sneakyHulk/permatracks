Radius = 3e-3; % m
Height = 4e-3; % m
Remanence = 1.35; % Tesla for N45 Magnet

lis3mdl_positions = [];
for i = 1:16
    lis3mdl_positions = [lis3mdl_positions struct('x', 40e-3 + floor((i - 1) / 4) * 30e-3, 'y', 45e-3 + mod(i - 1, 4) * 30e-3, 'z', 0)];
end

%crappy edge fartest away from the array:

holes_positions_direction_magnet_faces_x = [];
for x_pos = 1:10
    holes_positions_direction_magnet_faces_xx = [];
    for y_pos = 1:11
        holes_positions_direction_magnet_faces_xx =  [ holes_positions_direction_magnet_faces_xx struct('x', 37.5e-3 + (x_pos - 1) * 15e-3, 'y', 30e-3 + (y_pos - 1) * 15e-3, 'z', 55e-3)];
    end
    holes_positions_direction_magnet_faces_x = [holes_positions_direction_magnet_faces_x; holes_positions_direction_magnet_faces_xx];
end

holes_positions_direction_magnet_faces_y = [];
for x_pos = 1:11
    holes_positions_direction_magnet_faces_yy = [];
    for y_pos = 1:10
        holes_positions_direction_magnet_faces_yy = [holes_positions_direction_magnet_faces_yy struct('x', 30e-3 + (x_pos - 1) * 15e-3, 'y',  37.5e-3 + (y_pos - 1) * 15e-3, 'z', 55e-3)];
    end
    holes_positions_direction_magnet_faces_y = [holes_positions_direction_magnet_faces_y; holes_positions_direction_magnet_faces_yy];
end

holes_positions_direction_magnet_faces_z = [];
for x_pos = 1:11
    holes_positions_direction_magnet_faces_zz = [];
    for y_pos = 1:11
        holes_positions_direction_magnet_faces_zz = [holes_positions_direction_magnet_faces_zz struct('x', 30e-3 + (x_pos - 1) * 15e-3, 'y',  30e-3 + (y_pos - 1) * 15e-3, 'z', 55e-3)];
    end
    holes_positions_direction_magnet_faces_z = [holes_positions_direction_magnet_faces_z; holes_positions_direction_magnet_faces_zz];
end

function B = dipol(sensor_position, magnet_position, magnet_radius, magnet_height, magnet_remanence, magnet_magnetic_direction)
    magnetic_permeability = 4 * pi * 1e-7; % N/A²
    
    r = [magnet_position.x - sensor_position.x, magnet_position.y - sensor_position.y, magnet_position.z - sensor_position.z]; % Vector vom Sensor zum Magneten
    M = magnet_remanence / magnetic_permeability; % Magnetisierung
    m = magnet_magnetic_direction * M * pi * magnet_radius^2 * magnet_height;
    B = magnetic_permeability / (4 * pi) * ((3*r*(m*r')) ./ norm(r)^5 - m ./ norm(r)^3);

    %theta = 90 .* pi ./ 180;
    %phi = 0 * pi / 180;

    %m = magnet_remanence / 4 * magnet_radius^2 * magnet_height;
    
    %B2 = m * (3 * r(2) .* (...
    %    sin(theta) * cos(phi) * r(1)...
    %    + sin(theta) * sin(phi) * r(2)...
    %    + cos(theta) * r(3)) ./ norm(r)^5 ...
    %    - (sin(theta) * cos(phi)) ./ norm(r)^3) .* 1e6;
end

function B = biot_savat(sensor_position, magnet_position, magnet_radius, magnet_height, magnet_remanence) % magnet_magnetic_direction can only be [1, 0, 0].
    magnetic_permeability = 4 * pi * 1e-7; % N/A²
    M = magnet_remanence / magnetic_permeability; % Magnetisierung

    idet = @(a, b, c, theta, r, x) -sin(theta) .* (c - r .* sin(theta)) - cos(theta) .* (b - r .* cos(theta));
    jdet = @(a, b, c, theta, r, x) cos(theta) .* (a - x);
    kdet = @(a, b, c, theta, r, x) sin(theta) .* (a - x);
    
    ifun = @(a, b, c, theta, r, x) idet(a, b, c, theta, r, x) ...
        ./ ((a - x).^2 + (b - r .* cos(theta)).^2 + (c - r .* sin(theta)).^2).^(3/2);
    
    jfun = @(a, b, c, theta, r, x) jdet(a, b, c, theta, r, x) ...
        ./ ((a - x).^2 + (b - r .* cos(theta)).^2 + (c - r .* sin(theta)).^2).^(3/2);
    
    kfun = @(a, b, c, theta, r, x) kdet(a, b, c, theta, r, x) ...
        ./ ((a - x).^2 + (b - r .* cos(theta)).^2 + (c - r .* sin(theta)).^2).^(3/2);

    r = [magnet_position.x - sensor_position.x, magnet_position.y - sensor_position.y, magnet_position.z - sensor_position.z]; % Vector vom Sensor zum Magneten

    ifun2 = @(x, theta) ifun(r(1), r(2), r(3), theta, magnet_radius, x);
    jfun2 = @(x, theta) jfun(r(1), r(2), r(3), theta, magnet_radius, x);
    kfun2 = @(x, theta) kfun(r(1), r(2), r(3), theta, magnet_radius, x);
    
    
    B = [integral2(ifun2, -magnet_height / 2, magnet_height / 2, 0, 2 * pi), integral2(jfun2, -magnet_height / 2, magnet_height / 2, 0, 2 * pi), integral2(kfun2, -magnet_height / 2, magnet_height / 2, 0, 2 * pi)] ...
        * magnet_radius * magnetic_permeability / (4 * pi) * M;
end

dipol(lis3mdl_positions(1), holes_positions_direction_magnet_faces_x(10, 1), Radius, Height, Remanence, [1, 0, 0]) * 1e6
biot_savat(lis3mdl_positions(1), holes_positions_direction_magnet_faces_x(10, 1), Radius, Height, Remanence) * 1e6


calibration_values_x = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h26min51s_calibration_x.txt");
calibration_values_y = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h39min41s_calibration_y.txt");
calibration_values_z = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h45min44s_calibration_z.txt");
test_values = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h32min12s_(1-10_3).txt");

calibration_values = [...
    calibration_values_x.y0(2:end) calibration_values_x.x0(2:end) calibration_values_x.z0(2:end)
    calibration_values_y.y0(2:end) calibration_values_y.x0(2:end) calibration_values_y.z0(2:end)
    calibration_values_z.y0(2:end) calibration_values_z.x0(2:end) calibration_values_z.z0(2:end)
    ];

test_values = [...
    test_values.y0(2:end) test_values.x0(2:end) test_values.z0(2:end)
];

test_points = [
    holes_positions_direction_magnet_faces_x(1, 3)...
    holes_positions_direction_magnet_faces_x(2, 3)...
    holes_positions_direction_magnet_faces_x(3, 3)...
    holes_positions_direction_magnet_faces_x(4, 3)...
    holes_positions_direction_magnet_faces_x(5, 3)...
    holes_positions_direction_magnet_faces_x(6, 3)...
    holes_positions_direction_magnet_faces_x(7, 3)...
    holes_positions_direction_magnet_faces_x(8, 3)...
    holes_positions_direction_magnet_faces_x(9, 3)...
    holes_positions_direction_magnet_faces_x(10,3)...
    ];

test_directions = [
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [1 0 0])...
    ];


points = [
    holes_positions_direction_magnet_faces_x(1, 1)...
    holes_positions_direction_magnet_faces_x(1, 1)...
    holes_positions_direction_magnet_faces_x(10, 1)...
    holes_positions_direction_magnet_faces_x(10, 1)...
    holes_positions_direction_magnet_faces_x(10, 11)...
    holes_positions_direction_magnet_faces_x(10, 11)...
    holes_positions_direction_magnet_faces_x(1, 11)...
    holes_positions_direction_magnet_faces_x(1, 11)...
    holes_positions_direction_magnet_faces_x(5, 6)...
    holes_positions_direction_magnet_faces_x(5, 6)...
    holes_positions_direction_magnet_faces_y(1, 1)...
    holes_positions_direction_magnet_faces_y(1, 1)...
    holes_positions_direction_magnet_faces_y(11, 1)...
    holes_positions_direction_magnet_faces_y(11, 1)...
    holes_positions_direction_magnet_faces_y(11, 10)...
    holes_positions_direction_magnet_faces_y(11, 10)...
    holes_positions_direction_magnet_faces_y(1, 10)...
    holes_positions_direction_magnet_faces_y(1, 10)...
    holes_positions_direction_magnet_faces_y(5, 6)...
    holes_positions_direction_magnet_faces_y(5, 6)...
    holes_positions_direction_magnet_faces_z(1, 1)...
    holes_positions_direction_magnet_faces_z(1, 1)...
    holes_positions_direction_magnet_faces_z(11, 1)...
    holes_positions_direction_magnet_faces_z(11, 1)...
    holes_positions_direction_magnet_faces_z(11, 11)...
    holes_positions_direction_magnet_faces_z(11, 11)...
    holes_positions_direction_magnet_faces_z(1, 11)...
    holes_positions_direction_magnet_faces_z(1, 11)...
    holes_positions_direction_magnet_faces_z(6, 6)...
    holes_positions_direction_magnet_faces_z(6, 6)...
    ];

directions = [
    struct('direction', [1 0 0])...
    struct('direction', [-1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [-1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [-1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [-1 0 0])...
    struct('direction', [1 0 0])...
    struct('direction', [-1 0 0])...
    struct('direction', [0 1 0])...
    struct('direction', [0 -1 0])...
    struct('direction', [0 1 0])...
    struct('direction', [0 -1 0])...
    struct('direction', [0 1 0])...
    struct('direction', [0 -1 0])...
    struct('direction', [0 1 0])...
    struct('direction', [0 -1 0])...
    struct('direction', [0 1 0])...
    struct('direction', [0 -1 0])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    struct('direction', [0 0 1])...
    ];

dim = 3;
A = zeros(numel(points) * dim, 12);
b = zeros(1, numel(points) * dim);
b2 = zeros(1, numel(points) * dim);
for i = 1:numel(points)
   A((i - 1) * dim + 1, 1) = calibration_values(i, 1);
   A((i - 1) * dim + 1, 2) = calibration_values(i, 2);
   A((i - 1) * dim + 1, 3) = calibration_values(i, 3);
   A((i - 1) * dim + 1, 4) = 1;

   A((i - 1) * dim + 2, 5) = calibration_values(i, 1);
   A((i - 1) * dim + 2, 6) = calibration_values(i, 2);
   A((i - 1) * dim + 2, 7) = calibration_values(i, 3);
   A((i - 1) * dim + 2, 8) = 1;

   A((i - 1) * dim + 3, 9) =  calibration_values(i, 1);
   A((i - 1) * dim + 3, 10) = calibration_values(i, 2);
   A((i - 1) * dim + 3, 11) = calibration_values(i, 3);
   A((i - 1) * dim + 3, 12) = 1;
   
   b((i - 1) * dim + 1:(i - 1) * dim + 3) = dipol(lis3mdl_positions(1), points(i), Radius, Height, Remanence, directions(i).direction) * 1e6;
   b2((i - 1) * dim + 1:(i - 1) * dim + 3) = dipol(lis3mdl_positions(2), points(i), Radius, Height, Remanence, directions(i).direction) * 1e6;
end

x = A(:, :) \ b';
transform = [reshape(x, 4, [])'; 0 0 0 1];

transform2 = ... done with ransac
[[ 1.15627603e+00 -3.37779947e-02  3.70777311e-02 -4.62837789e+01]
 [-1.97323211e-03  1.17287436e+00 -2.43311018e-02 -4.51719858e+01]
 [-9.89901309e-02  5.01613850e-02  1.14784453e+00  7.78668405e+01]
 [ 0.00000000e+00  0.00000000e+00  0.00000000e+00  1.00000000e+00]];

for i = 1:10
    gt(i, :) = dipol(lis3mdl_positions(1), test_points(i), Radius, Height, Remanence, test_directions(i).direction) * 1e6;
    gt2(i, :) = biot_savat(lis3mdl_positions(1), test_points(i), Radius, Height, Remanence) * 1e6;
    v1(i, :) = transform * [test_values(i, :) 1]'; % static and linear
    %v2(i, :) = [test_values.y0(i) test_values.x0(i) test_values.z0(i)] - [test_values.y0(1) test_values.x0(1) test_values.z0(1)]; % only static magnetci field
    v3(i, :) = transform2 * [test_values(i, :) 1]'; % static and linear and ransac

    jac = jacobianest(@(x) dipol(lis3mdl_positions(1), struct('x', x(1), 'y', x(2), 'z', x(3)), Radius, Height, Remanence, test_directions(i).direction), [test_points(i).x test_points(i).y test_points(i).z])
    err_uT1(i, :) = gt(i, :) - v1(i, 1:3);
    err_mm1(i, :) = abs(1./ diag(jac))' .* abs(gt(i, :) - v1(i, 1:3)) * 1e-6 * 1e3;

    %err_uT2(i, :) = gt(i, :) - v2(i, 1:3);
    %err_mm2(i, :) = abs(1./ diag(jac))' .* abs(gt(i, :) - v2(i, 1:3)) * 1e-6 * 1e3;

    err_uT3(i, :) = gt(i, :) - v3(i, 1:3);
    err_mm3(i, :) = abs(1./ diag(jac))' .* abs(gt(i, :) - v3(i, 1:3)) * 1e-6 * 1e3;

    %options = optimoptions('fsolve');
    %err_mm2(i, :) = (fsolve(@(x) dipol(lis3mdl_positions(1), struct('x', x(1), 'y', x(2), 'z', x(3)), Radius, Height, Remanence, test_directions(i).direction) ...
    %+ 0.1 ...
    %,[test_points(i).x test_points(i).y test_points(i).z], options));
    %%+ dipol(lis3mdl_positions(1), struct('x', test_points(i).x, 'y', test_points(i).y, 'z', test_points(i).z), Radius, Height, Remanence, test_directions(i).direction)...
    %    ...    - (err_uT(i, :) / 1e6) ...
    %err_mm3(i, :) = dipol(lis3mdl_positions(1), struct('x', err_mm2(i, 1), 'y', err_mm2(i, 2), 'z', err_mm2(i, 3)), Radius, Height, Remanence, test_directions(i).direction) * 1e6
end

err_uT1
err_mm1

if false
figure
title('Testdata (1:10, 3, [1 0 0])')
ylabel('Magnetic flux density [µT] in z-direction')

n = numel(test_points);
t = tiledlayout(3, 3 * n, 'TileSpacing','compact','Padding', 'compact');

for i = 1:9
    bgax(i) = nexttile((i - 1) * n + 1);
    bgax(i).Layout.Tile = (i - 1) * n + 1;
    bgax(i).XTick = [];
    bgax(i).YTick = [];
    bgax(i).Box = 'off';
    bgax(i).Layout.TileSpan = [1 n];
end

for i = 1:n
    Xx = linspace(min([test_points(i).x]) - 4e-3, max([test_points(i).x]) + 4e-3, 100);
    
    for j = 1:numel(Xx)
        Yx(j, :) = biot_savat(lis3mdl_positions(1), struct('x', Xx(j), 'y', test_points(i).y, 'z', test_points(i).z), Radius, Height, Remanence) * 1e6;
    end

    ax1(i) = axes(t);
    plot(ax1(i), Xx, Yx(:, 1), 'k');
    hold on;
    plot(ax1(i), test_points(i).x, v1(i, 1), 'r*')
    ax1(i).YAxis.Visible = 'off';
    ax1(i).Box = 'off';
    ax1(i).Layout.Tile = i;

    ax2(i) = axes(t);
    plot(ax2(i), Xx, Yx(:, 2), 'k');
    hold on;
    plot(ax2(i), test_points(i).x, v1(i, 2), 'r*')
    ax2(i).YAxis.Visible = 'off';
    ax2(i).Box = 'off';
    ax2(i).Layout.Tile = n + i;

    ax3(i) = axes(t);
    plot(ax3(i), Xx, Yx(:, 3), 'k');
    hold on;
    plot(ax3(i), test_points(i).x, v1(i, 3), 'r*')
    ax3(i).YAxis.Visible = 'off';
    ax3(i).Box = 'off';
    ax3(i).Layout.Tile = 2 * n + i;
end

ax1(1).YAxis.Visible = 'on';
ax2(1).YAxis.Visible = 'on';
ax3(1).YAxis.Visible = 'on';
linkaxes(ax1, 'y');
linkaxes(ax2, 'y');
linkaxes(ax3, 'y');

for i = 1:numel(test_points)
    Xy = linspace(min([test_points(i).y]) - 4e-3, max([test_points(i).y]) + 4e-3, 100);

    for j = 1:numel(Xy)
        Yy(j, :) = biot_savat(lis3mdl_positions(1), struct('x', test_points(i).x, 'y', Xy(j), 'z', test_points(i).z), Radius, Height, Remanence) * 1e6;
    end

    ax4(i) = axes(t);
    plot(ax4(i), Xy, Yy(:, 1), 'k');
    hold on;
    plot(ax4(i), test_points(i).y, v1(i, 1), 'r*')
    ax4(i).YAxis.Visible = 'off';
    ax4(i).Box = 'off';
    ax4(i).Layout.Tile = 3 * n + i;

    ax5(i) = axes(t);
    plot(ax5(i), Xy, Yy(:, 2), 'k');
    hold on;
    plot(ax5(i), test_points(i).y, v1(i, 2), 'r*')
    xline(ax5(i), test_points(i).y)
    ax5(i).YAxis.Visible = 'off';
    ax5(i).Box = 'off';
    ax5(i).Layout.Tile = 4 * n + i;

    ax6(i) = axes(t);
    plot(ax6(i), Xy, Yy(:, 3), 'k');
    hold on;
    plot(ax6(i), test_points(i).y, v1(i, 3), 'r*')
    ax6(i).YAxis.Visible = 'off';
    ax6(i).Box = 'off';
    ax6(i).Layout.Tile = 5 * n + i;
end

ax4(1).YAxis.Visible = 'on';
ax5(1).YAxis.Visible = 'on';
ax6(1).YAxis.Visible = 'on';
linkaxes(ax4, 'y');
linkaxes(ax5, 'y');
linkaxes(ax6, 'y');


for i = 1:numel(test_points)
    Xz = linspace(min([test_points.z]) - 0.05, max([test_points.z]) + 0.05, 100);
    
    for j = 1:numel(Xx)
        Yz(j, :) = biot_savat(lis3mdl_positions(1), struct('x', test_points(i).x, 'y', test_points(i).y, 'z', Xz(j)), Radius, Height, Remanence) * 1e6;
    end

    ax7(i) = axes(t);
    plot(ax7(i), Xz, Yz(:, 1), 'k');
    hold on;
    plot(ax7(i), test_points(i).z, v1(i, 1), 'r*')
    ax7(i).YAxis.Visible = 'off';
    ax7(i).Box = 'off';
    ax7(i).Layout.Tile = 6 * n + i;
    ylim(ax7(i),[-70 70])

    ax8(i) = axes(t);
    plot(ax8(i), Xz, Yz(:, 2), 'k');
    hold on;
    plot(ax8(i), test_points(i).z, v1(i, 2), 'r*')
    ax8(i).YAxis.Visible = 'off';
    ax8(i).Box = 'off';
    ax8(i).Layout.Tile = 7 * n + i;
    ylim(ax8(i),[-70 70])

    ax9(i) = axes(t);
    plot(ax9(i), Xz, Yz(:, 3), 'k');
    hold on;
    plot(ax9(i), test_points(i).z, v1(i, 3), 'r*')
    ax9(i).YAxis.Visible = 'off';
    ax9(i).Box = 'off';
    ax9(i).Layout.Tile = 8 * n + i;
    ylim(ax9(i),[-70 70])
end

ax7(1).YAxis.Visible = 'on';
ax8(1).YAxis.Visible = 'on';
ax9(1).YAxis.Visible = 'on';
linkaxes(ax7, 'y');
linkaxes(ax8, 'y');
linkaxes(ax9, 'y');
end



%AA = [11 12 13 14;21 22 23 24; 31 32 33 34];
%AA = reshape(AA', 1, []);
%AA = [reshape(AA, 4, [])'; 0 0 0 1];
%
%
%
%
%test_values = readtable("../../result/mag_data_LIS3MDL_ARRAY_mean_2025Mar14_15h32min12s_(1-10_3).txt");
%
%
%%(res1 - no_influence2) * [0 1 0; -1 0 0; 0 0 -1]
%%(res2 - no_influence2) * [0 1 0; -1 0 0; 0 0 -1]
%
%dipol(lis3mdl_positions(1), struct('x', 0e-3, 'y', 45e-3, 'z', 0), Radius, Height, Remanence, [-1, 0, 0]) * 1e6



