syms Gx Gy Gz x1 y1 z1 m1 theta1 phi1


% Extract sensor positions
sx = 40e-3 + floor((linspace(1, 16, 16) - 1) / 4) * 30e-3;
sy = 45e-3 + mod(linspace(1, 16, 16) - 1, 4) * 30e-3;
sz = zeros(1, 16);

% Calculate relative positions
ri1x = x1 - sx;
ri1y = y1 - sy;
ri1z = z1 - sz;

% Initialize magnetic field components
Bi1x = sym(zeros(size(ri1x)));
Bi1y = sym(zeros(size(ri1y)));
Bi1z = sym(zeros(size(ri1z)));

% Calculate magnetic field components
for i = 1:length(ri1x)
    r1x = ri1x(i);
    r1y = ri1y(i);
    r1z = ri1z(i);

    r = sqrt(r1x^2 + r1y^2 + r1z^2);

    Bi1x(i) = Gx + m1 * (3 * r1x * (sin(theta1) * cos(phi1) * r1x + ...
                                    sin(theta1) * sin(phi1) * r1y + ...
                                    cos(theta1) * r1z) / r^5 - ...
                         sin(theta1) * cos(phi1) / r^3);

    Bi1y(i) = Gy + m1 * (3 * r1y * (sin(theta1) * cos(phi1) * r1x + ...
                                    sin(theta1) * sin(phi1) * r1y + ...
                                    cos(theta1) * r1z) / r^5 - ...
                         sin(theta1) * sin(phi1) / r^3);

    Bi1z(i) = Gz + m1 * (3 * r1z * (sin(theta1) * cos(phi1) * r1x + ...
                                    sin(theta1) * sin(phi1) * r1y + ...
                                    cos(theta1) * r1z) / r^5 - ...
                         cos(theta1) / r^3);
end

% Combine magnetic field components into a single model vector
model = [Bi1x, Bi1y, Bi1z];


model_jac = jacobian(model, [Gx Gy Gz x1 y1 z1 theta1 phi1]);

matlabFunction(model, 'file', 'myFunction');
codegen -config:lib myFunction -args {0, 0, 0, 0, 0, 0, 0, 0, 0} -c

matlabFunction(model_jac, 'file', 'myFunction2');
codegen -config:lib myFunction2 -args {0, 0, 0, 0, 0, 0} -c

