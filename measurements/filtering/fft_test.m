% Since the sensor has a high level of noise, which leads to high position noise, and the sensor provides measured values at a higher frequency than the product actually requires, filtering can help to reduce the noise.

a = measurements1{1}.z;
L = numel(measurements1{1}.z);
t = measurements1{1}.timestamp;

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

% Design a low-pass filter
fc = 2; % Cutoff frequency (Hz)
filterOrder = 5;
df = designfilt('lowpassfir', 'FilterOrder', filterOrder, 'CutoffFrequency', fc, 'SampleRate', Fs);
lowpassfir = filter(df, a);
df2 = designfilt('lowpassiir', 'FilterOrder', filterOrder, 'HalfPowerFrequency', fc, 'SampleRate', Fs);
lowpassiir = filter(df2, a);

sgolayfilter = sgolayfilt(a, 2, 11);
movmeanfiler = movmean(a, 4);

decimatefilter = decimate(a, 4);
t_decimated = downsample(t, 4);


medianfilter = medfilt1(a, 5);

% Plot the original and filtered signals
figure;
p = subplot(7,1,1);
plot(t, a);
title(['Original Noisy Signal std = ' num2str(std(a(40:end)))]);
xlabel('Time (s)');
ylabel('Amplitude');
yl = get(p, 'Ylim');

subplot(7,1,2);
plot(t, lowpassfir);
title(['Filtered Signal lowpassfir std = ' num2str(std(lowpassfir(40:end)))]);
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,3);
plot(t, lowpassiir);
title(['Filtered Signal lowpassiir std = ' num2str(std(lowpassiir(40:end)))]);
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,4);
plot(t, sgolayfilter);
title(['Filtered Signal sgolayfilt std = ' num2str(std(sgolayfilter(40:end)))]);
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,5);
plot(t, movmeanfiler);
title(['Filtered Signal movmean std = ' num2str(std(movmeanfiler(40:end)))]);
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,6);
plot(t_decimated, decimatefilter);
title(['Filtered Signal decimate std = ' num2str(std(decimatefilter(30:end)))]);
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);

subplot(7,1,7);
plot(t, medianfilter);
title(['Filtered Signal medfilt1 std = ' num2str(std(medianfilter(30:end)))]);
xlabel('Time (s)');
ylabel('Amplitude');
ylim(yl);