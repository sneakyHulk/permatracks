values1 = readtable(fullfile('../../result/', 'mag_data_MMC5983MA_2025Mar13_13h56min14s_fast_lay.txt'));
values2 = readtable(fullfile('../../result/', 'mag_data_MMC5983MA_2025Mar13_14h43min34s_fast_switch_90to120mm.txt'));
values3 = readtable(fullfile('../../result/', 'mag_data_MMC5983MA_2025Mar13_14h49min48s_fast_lay2.txt'));
values4 = readtable(fullfile('../../result/', 'mag_data_MMC5983MA_2025Mar13_15h3min49s_fast_lay_90mm.txt'));



values1_mean = mean(values1.z)
values2_mean = mean(values2.z)
values3_mean = mean(values3.z)
values4_mean = mean(values4.z)

values1_std = std(values1.z)
values2_std = std(values2.z)
values3_std = std(values3.z)
values4_std = std(values4.z)

