%% COMPSYS 301 - Light sensor capture analysis (R_sh = 27k, projector on)
% Analyses oscilloscope CSV exports of the sensor node voltage, extracts the
% numeric requirements for the signal-conditioning design (DC level, swing,
% spectrum, post-filter amplitude), and visualises everything in one figure.
%
% Expected CSV format (scope export): 2 header lines, then "time,volts".
% Put this script in the same folder as the CSVs, or edit the paths below.

clear; clc; close all;

%% ---------------- Parameters (edit these) ----------------
files  = { '27K_lighton_white.csv', '27K_lighton_black.csv' };
labels = { 'white surface', 'black surface' };
cols   = [0.23 0.45 0.70;    % blue  - white surface
          0.85 0.55 0.10];   % amber - black surface

fc_hp   = 15.9;   % designed high-pass corner, Hz  (R1C1 = 10k * 1uF)
Vref    = 2.5;    % mid-rail bias the HP resistor returns to, V
Vcm_max = 3.5;    % LM324 input CM / output ceiling on 5 V supply, V

%% ---------------- Load ----------------
n  = numel(files);
T  = cell(1,n); V = cell(1,n);
for k = 1:n
    M = readmatrix(files{k}, 'NumHeaderLines', 2);
    M = M(~any(isnan(M),2), :);          % drop rows with missing samples
    T{k} = M(:,1);  V{k} = M(:,2);
end
fs = 1 / median(diff(T{1}));             % sample rate from time column

%% ---------------- Analyse ----------------
fprintf('fs = %.0f Hz, window = %.0f ms\n\n', fs, 1e3*(T{1}(end)-T{1}(1)));
res = struct();
for k = 1:n
    v = V{k};
    res(k).mean = mean(v);
    res(k).min  = min(v);   res(k).max = max(v);
    res(k).vpp  = max(v) - min(v);

    % Single-sided amplitude spectrum of the AC part
    x   = v - mean(v);
    N   = numel(x);
    w   = 0.5*(1 - cos(2*pi*(0:N-1)'/(N-1)));        % Hann window (no toolbox)
    Y   = fft(x .* w);
    f   = (0:floor(N/2))' * fs/N;
    A   = abs(Y(1:floor(N/2)+1)) * 2/sum(w);         % amplitude, window-corrected
    res(k).f = f;  res(k).A = A;

    % Dominant component & fundamental period estimate
    [~, ip] = max(A);
    res(k).fdom = f(ip);

    % Simulate the first-order HP (bilinear-free discrete form):
    % y(nn) = a*( y(nn-1) + x(nn) - x(nn-1) ),  a = RC/(RC+Ts)
    RC = 1/(2*pi*fc_hp);  a = RC/(RC + 1/fs);
    y  = zeros(N,1);
    for nn = 2:N
        y(nn) = a*( y(nn-1) + x(nn) - x(nn-1) );
    end
    y = y(floor(N/2):end);               % keep settled half only
    res(k).y     = y;
    res(k).ypp   = max(y) - min(y);
    res(k).ypos  = max(y);  res(k).yneg = min(y);

    fprintf('%s:\n', labels{k});
    fprintf('  DC mean = %.3f V   raw min/max = %.3f / %.3f V   Vpp = %.3f V\n', ...
            res(k).mean, res(k).min, res(k).max, res(k).vpp);
    fprintf('  dominant component = %.1f Hz (amp %.3f V)\n', res(k).fdom, max(res(k).A));
    fprintf('  post-HP swing about V_ref: +%.3f / %.3f V  (pp = %.3f V)\n', ...
            res(k).ypos, res(k).yneg, res(k).ypp);
    fprintf('  peak after biasing at %.2f V -> %.3f V  (LM324 limit %.2f V)\n\n', ...
            Vref, Vref + res(k).ypos, Vcm_max);
end
fprintf('AC contrast (white/black post-HP pp): %.2f x\n', res(1).ypp / res(2).ypp);

%% ---------------- Visualise ----------------
fig = figure('Color','w','Position',[80 80 1050 780]);
tl  = tiledlayout(2,2,'TileSpacing','compact','Padding','compact');
title(tl, sprintf('Sensor node, R_{sh} = 27 k\\Omega, projector on   (f_s = %.0f Hz)', fs));

% (1) Full captures
nexttile; hold on; grid on;
for k = 1:n, plot(1e3*T{k}, V{k}, 'Color', cols(k,:)); end
yline(Vcm_max,'r--','LM324 limit');
xlabel('time (ms)'); ylabel('V'); title('Raw capture');
legend(labels,'Location','best');

% (2) Zoom: two fundamental cycles
nexttile; hold on; grid on;
Tz = 2/res(1).fdom;                      % two periods of the dominant component
iz = T{1} >= T{1}(1) & T{1} <= T{1}(1) + Tz*2;
for k = 1:n, plot(1e3*T{k}(iz), V{k}(iz), '-o', 'Color', cols(k,:), 'MarkerSize', 2); end
xlabel('time (ms)'); ylabel('V');
title(sprintf('Zoom: pulse shape (dominant %.0f Hz)', res(1).fdom));

% (3) Spectrum
nexttile; hold on; grid on;
for k = 1:n, plot(res(k).f, res(k).A, 'Color', cols(k,:)); end
xlim([0 1000]);
xline(60,  ':', '60 Hz');
xline(120, ':', '120 Hz');
xline(fc_hp, '--', sprintf('f_c = %.0f Hz', fc_hp));
xlabel('frequency (Hz)'); ylabel('amplitude (V)');
title('Single-sided amplitude spectrum (DC removed)');

% (4) Post-HP output, biased on Vref
nexttile; hold on; grid on;
for k = 1:n
    ty = (0:numel(res(k).y)-1)'/fs;
    plot(1e3*ty, Vref + res(k).y, 'Color', cols(k,:));
end
yline(Vref,   'k--', 'V_{ref}');
yline(Vcm_max,'r--', 'LM324 limit');
xlabel('time (ms)'); ylabel('V');
title(sprintf('Simulated HP output (f_c = %.1f Hz), biased at %.2f V', fc_hp, Vref));

% Uncomment to save for the logbook:
% exportgraphics(fig, 'sensor_capture_analysis.png', 'Resolution', 200);