% ---- pick one ----
fname = 'capture_square.csv';   % remember: contents are SWAPPED vs names
% fname = 'capture_sine.csv';

[i, adc, dac] = textread(fname, '%d, %f, %f');

Ts = 15.08e-6;                  % measured, from your DebugPin
t  = i * Ts * 1e3;              % ms
va = adc * 0.5e-3;              % counts -> V
vd = dac * 16e-3;               % code   -> V

figure;
stairs(t, vd, 'r', 'LineWidth', 1); hold on;
plot(t, va, 'b');
xlabel('t (ms)'); ylabel('V'); legend('DAC','ADC'); grid on;

fprintf('mean %.4f V   min %.3f   max %.3f\n', mean(va), min(va), max(va));

N = length(va); Y = abs(fft(va - mean(va)));
f = (0:N-1)/(N*Ts);
[~,k] = max(Y(2:floor(N/2)));
fprintf('dominant frequency: %.0f Hz\n', f(k+1));