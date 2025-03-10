% Sample parameters
Fs = 1000; % Sampling frequency (Hz)
T = 1/Fs; % Sampling period (s)
L = 1500; % Length of signal
t = (0:L-1)*T; % Time vector

% Generate a sample signal (e.g., a sum of sine waves)
f1 = 50; % Frequency of the first sine wave (Hz)
f2 = 120; % Frequency of the second sine wave (Hz)
a = 0.7*sin(2*pi*f1*t) + sin(2*pi*f2*t); % Sensor signal
a = measurements1{1}.z;
L = numel(measurements1{1}.z);

% Compute the FFT
NFFT = 2^nextpow2(L); % Next power of 2 from length of signal
Y = fft(a, NFFT)/L;
f = Fs/2*linspace(0, 1, NFFT/2+1);

% Plot the single-sided amplitude spectrum
figure;
plot(f, 2*abs(Y(1:NFFT/2+1)));
title('Single-Sided Amplitude Spectrum of a');
xlabel('Frequency (f)');
ylabel('|Y(f)|');

a = measurements1{1}.z;
t = measurements1{1}.timestamp;
Fs = 80;

% Design a low-pass filter
fc = 10; % Cutoff frequency (Hz)
filterOrder = 5;
df2 = designfilt('lowpassiir', 'FilterOrder', filterOrder, 'HalfPowerFrequency', fc, 'SampleRate', Fs);
df = designfilt('lowpassfir', 'FilterOrder', filterOrder, 'CutoffFrequency', fc, 'SampleRate', Fs);

smoothedMagData = sgolayfilt(a, 2, 11);
smoothedMagData2 = movmean(a, 4);

decimated_data = decimate(a, 4);
t_decimated = downsample(t, 4);

cicDecim = dsp.CICDecimator(4, 3);
decimated_data2 = cicDecim(a);


% Apply the filter to the signal
filteredSignal = filter(df, a);
filteredSignal2 = filter(df2, a);

% Plot the original and filtered signals
figure;
p = subplot(7,1,1);
plot(t, a);
title('Original Noisy Signal');
xlabel('Time (s)');
ylabel('Amplitude');
yl = get(p, 'Ylim');

subplot(7,1,2);
plot(t, filteredSignal);
title('Filtered Signal');
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,3);
plot(t, filteredSignal2);
title('Filtered Signal');
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,4);
plot(t, smoothedMagData);
title('Filtered Signal');
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,5);
plot(t, smoothedMagData2);
title('Filtered Signal');
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,6);
plot(t_decimated, decimated_data);
title('Filtered Signal');
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,7);
plot(t_decimated, decimated_data2);
title('Filtered Signal');
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

std(a(300:end))
std(filteredSignal(300:end))
std(filteredSignal2(300:end))
std(smoothedMagData(300:end))
std(smoothedMagData2(300:end))
std(decimated_data(300:end))
std(decimated_data2(300:end))
