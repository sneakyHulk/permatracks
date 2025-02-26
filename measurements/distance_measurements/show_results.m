data = fileread('distances_lis3mdl.json');
data = jsondecode(data);

distance_fieldnames = fieldnames(data);

measurement_values = [];
distances = [];

for i = 1:numel(distance_fieldnames)
    measurement_values = [measurement_values, data.(distance_fieldnames{i})];  % Append data
    distance = distance_fieldnames{i};
    distance = distance(2:end-2);
    distances = [distances; str2num(distance)];



end

[distances, index] = sort(distances);
measurement_values = measurement_values(:, index);

measurement_values_means = mean(measurement_values);
measurement_values_differences = diff(-measurement_values_means);
measurement_values_std = std(measurement_values);
distances_differences = diff(distances);

hold on
boxplot(measurement_values(:, 1), 'Positions', 1)
boxplot(measurement_values(:, 2:numel(distance_fieldnames)), 'Positions', 2:numel(distance_fieldnames))

plot(2:numel(distance_fieldnames), measurement_values_means(2:end))

for pos = 2:numel(distance_fieldnames)-1
    text(.5+pos, measurement_values_means(pos) - measurement_values_differences(pos) / 2 + 10, ['±' num2str(ceil(distances_differences(pos) / measurement_values_differences(pos) * max(measurement_values_std)*10)/10), 'mm'])
end
hold off;

xticks(1:numel(distance_fieldnames));
xticklabels(["kein Magnet"; string(distances(2:end))]);
xlabel('Abstand zum Sensor [mm]')
ylabel('Magnetische Flussdichte [µT]')





