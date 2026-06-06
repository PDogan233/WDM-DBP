%% DP_FiberTest.m
% MATLAB visualization script for DP_Fiber Agent.
% Reads .dat files saved by DP_FiberTest.cpp (visualization data section).
% Covers time-domain and frequency-domain plots for pure loss and pure
% dispersion tests.
%
% Usage:
%   1. Build & run DP_FiberTest.exe  (generates .dat files)
%   2. In MATLAB: >> DP_FiberTest
%
% .dat format (cmat 2 x Nt):
%   Row 1: X-pol, 2*Nt reals (Re,Im alternating)
%   Row 2: Y-pol, 2*Nt reals (Re,Im alternating)

%% Parameters — must match DP_FiberTest.cpp visualization section
Nt   = 1024;              % number of time samples
fs   = 100e9;             % sampling rate [Hz]
dt   = 1 / fs;            % sampling interval [s]
t    = (0 : Nt-1)' * dt;  % time axis [s] (column vector)
df   = fs / Nt;           % frequency spacing [Hz]
f    = (-Nt/2 : Nt/2-1)' * df;  % frequency axis [Hz] (fftshift order)

% ============================================================
% Figure 1: Pure Loss Test
% Purpose: verify exponential field attenuation exp(-alphaNp*L/2).
% Expected: output = input * exp(-alphaNp*L/2), waveform unchanged.
% Verification metric: attenuation ratio (measured vs expected).
% ============================================================
fprintf('=== DP_Fiber: Pure Loss Visualization ===\n');

[sigX_in, sigY_in]   = readCmat('FiberInput_PureLoss.dat');
[sigX_out, sigY_out] = readCmat('FiberOutput_PureLoss.dat');

% Physical parameters (match C++ test)
alpha_dBpm = 0.2e-3;        % [dB/m]
lSpan      = 100e3;         % [m]
alphaNp    = alpha_dBpm * log(10) / 10;  % [Neper/m]

% Attenuation metric
powerIn  = sum(abs(sigX_in).^2  + abs(sigY_in).^2);
powerOut = sum(abs(sigX_out).^2 + abs(sigY_out).^2);
ratioMeasured = sqrt(powerOut / powerIn);
ratioExpected = exp(-alphaNp * lSpan / 2);
relErr = abs(ratioMeasured - ratioExpected) / ratioExpected;

fprintf('  Field attenuation: measured=%.6f  expected=%.6f  relErr=%.2e\n', ...
    ratioMeasured, ratioExpected, relErr);

figure('Name', 'DP_Fiber: Pure Loss', 'Position', [100, 100, 1200, 800]);

subplot(2,2,1);
plot(t*1e9, abs(sigX_in).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(sigX_out).^2, 'r--','LineWidth', 1.2);
xlabel('Time (ns)'); ylabel('Power (W)');
title('X-Pol: Intensity (Time Domain)');
legend('Input', 'Output'); grid on;

subplot(2,2,2);
plot(t*1e9, abs(sigY_in).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(sigY_out).^2, 'r--','LineWidth', 1.2);
xlabel('Time (ns)'); ylabel('Power (W)');
title('Y-Pol: Intensity (Time Domain)');
legend('Input', 'Output'); grid on;

subplot(2,2,3);
specIn  = fftshift(abs(fft(sigX_in)).^2);
specOut = fftshift(abs(fft(sigX_out)).^2);
plot(f*1e-9, 10*log10(specIn  + eps), 'b-', 'LineWidth', 1); hold on;
plot(f*1e-9, 10*log10(specOut + eps), 'r--','LineWidth', 1);
xlabel('Frequency (GHz)'); ylabel('Power (dB)');
title('X-Pol: Spectrum');
legend('Input', 'Output'); grid on; xlim([-200 200]);

subplot(2,2,4);
specIn  = fftshift(abs(fft(sigY_in)).^2);
specOut = fftshift(abs(fft(sigY_out)).^2);
plot(f*1e-9, 10*log10(specIn  + eps), 'b-', 'LineWidth', 1); hold on;
plot(f*1e-9, 10*log10(specOut + eps), 'r--','LineWidth', 1);
xlabel('Frequency (GHz)'); ylabel('Power (dB)');
title('Y-Pol: Spectrum');
legend('Input', 'Output'); grid on; xlim([-200 200]);

sgtitle(sprintf('Pure Loss  (alpha=%.1e dB/m, lSpan=%d km)  |  Attenuation=%.4f (exp=%.4f)  relErr=%.2e', ...
    alpha_dBpm, lSpan/1e3, ratioMeasured, ratioExpected, relErr));

% ============================================================
% Figure 2: Pure Dispersion Test
% Purpose: verify energy conservation and pulse broadening.
% Expected: energy conserved, pulse broadens in time, spectrum
%   magnitude unchanged (dispersion is all-pass in frequency).
% Verification metric: energy relative difference, FWHM change.
% ============================================================
fprintf('\n=== DP_Fiber: Pure Dispersion Visualization ===\n');

[sigX_in, sigY_in]   = readCmat('FiberInput_PureDispersion.dat');
[sigX_out, sigY_out] = readCmat('FiberOutput_PureDispersion.dat');

energyIn  = sum(abs(sigX_in).^2  + abs(sigY_in).^2);
energyOut = sum(abs(sigX_out).^2 + abs(sigY_out).^2);
relDiff   = abs(energyOut - energyIn) / energyIn;
fprintf('  Energy conservation: relDiff=%.2e  (expected ~0)\n', relDiff);

fwhmInX  = computeFWHM(t, abs(sigX_in).^2);
fwhmOutX = computeFWHM(t, abs(sigX_out).^2);
fprintf('  X-Pol FWHM: in=%.1f ps  out=%.1f ps\n', fwhmInX*1e12, fwhmOutX*1e12);

Dispersion = 16.5e-6;  % D [s/m^2], match C++ test default

figure('Name', 'DP_Fiber: Pure Dispersion', 'Position', [150, 150, 1200, 800]);

subplot(2,2,1);
plot(t*1e9, abs(sigX_in).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(sigX_out).^2, 'r--','LineWidth', 1.2);
xlabel('Time (ns)'); ylabel('Power (W)');
title(sprintf('X-Pol: Intensity  (FWHM %.0f -> %.0f ps)', fwhmInX*1e12, fwhmOutX*1e12));
legend('Input', 'Output'); grid on;

subplot(2,2,2);
plot(t*1e9, abs(sigY_in).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(sigY_out).^2, 'r--','LineWidth', 1.2);
xlabel('Time (ns)'); ylabel('Power (W)');
title('Y-Pol: Intensity (Time Domain)');
legend('Input', 'Output'); grid on;

subplot(2,2,3);
magIn  = fftshift(abs(fft(sigX_in)));
magOut = fftshift(abs(fft(sigX_out)));
plot(f*1e-9, magIn,  'b-', 'LineWidth', 1); hold on;
plot(f*1e-9, magOut, 'r--','LineWidth', 1);
xlabel('Frequency (GHz)'); ylabel('Magnitude');
title('X-Pol: |Spectrum| (should be identical)');
legend('Input', 'Output'); grid on; xlim([-200 200]);

subplot(2,2,4);
phaseIn  = fftshift(angle(fft(sigX_in)));
phaseOut = fftshift(angle(fft(sigX_out)));
phaseDiff = wrapToPi(phaseOut - phaseIn);
plot(f*1e-9, phaseDiff, 'k-', 'LineWidth', 1);
xlabel('Frequency (GHz)'); ylabel('Phase Difference (rad)');
title('X-Pol: Phase Difference (dispersion signature)'); grid on; xlim([-200 200]);

sgtitle(sprintf('Pure Dispersion  (D=%.1e s/m^2, lSpan=%d km)  |  Energy relDiff=%.2e', ...
    Dispersion, lSpan/1e3, relDiff));

% ============================================================
% Local functions (must be at end of script per MATLAB rules)
% ============================================================

function [sigX, sigY] = readCmat(filename)
    % Read cmat (2 x Nt) from .dat file, return column vectors.
    % .dat format: 2 rows, each with 2*Nt reals (Re,Im alternating).
    data = load(filename);
    Ncols = size(data, 2);
    NtLocal = Ncols / 2;
    sigX = data(1, 1:2:end).' + 1j * data(1, 2:2:end).';
    sigY = data(2, 1:2:end).' + 1j * data(2, 2:2:end).';
end

function w = computeFWHM(timeVec, powerVec)
    % Full-width at half-maximum of a pulse power profile.
    [pk, idx] = max(powerVec);
    half = pk / 2;
    left  = find(powerVec(1:idx) <= half, 1, 'last');
    right = find(powerVec(idx:end) <= half, 1, 'first');
    if isempty(left),  left  = 1; end
    if isempty(right), right = length(powerVec) - idx + 1; end
    w = timeVec(idx + right - 1) - timeVec(left);
end
