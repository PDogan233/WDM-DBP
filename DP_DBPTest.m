%% DP_DBPTest.m
% MATLAB visualization script for DP_DBP Agent.
% Reads .dat files saved by DP_FiberTest.cpp (visualization data section).
% Covers round-trip DBP compensation and multi-span back-propagation.
%
% Usage:
%   1. Build & run DP_FiberTest.exe  (generates .dat files)
%   2. In MATLAB: >> DP_DBPTest
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
% Figure 1: Single-span Round-trip DBP
% Purpose: verify DBP compensates fiber impairments.
% Expected: DBP-compensated signal ≈ original signal (NMSE ~ 0).
%   Fiber output differs significantly from original
%   (due to dispersion + nonlinearity combined).
% Verification metric: NMSE (original vs DBP-compensated).
% ============================================================
fprintf('=== DP_DBP: Single-Span Round-Trip Visualization ===\n');

[origX, origY]             = readCmat('DBP_OriginalSignal.dat');
[fiberX, fiberY]           = readCmat('DBP_FiberOutput.dat');
[compensatedX, compensatedY] = readCmat('DBP_Compensated.dat');

nmseRoundTrip = computeNMSE(origX, origY, compensatedX, compensatedY);
nmseFiberOnly = computeNMSE(origX, origY, fiberX, fiberY);
fprintf('  NMSE (fiber output  vs original): %.4e\n', nmseFiberOnly);
fprintf('  NMSE (DBP compensated vs original): %.4e  (expected ~0)\n', nmseRoundTrip);

lSpan = 10e3;  % [m], match C++ test

figure('Name', 'DP_DBP: Single-Span Round-Trip', 'Position', [100, 100, 1200, 800]);

subplot(2,2,1);
plot(t*1e9, abs(origX).^2,        'b-',  'LineWidth', 1.2); hold on;
plot(t*1e9, abs(fiberX).^2,       'r--', 'LineWidth', 1.0);
plot(t*1e9, abs(compensatedX).^2, 'g-.', 'LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)');
title('X-Pol: Intensity (Time Domain)');
legend('Original', 'Fiber out', 'DBP out'); grid on;

subplot(2,2,2);
plot(t*1e9, abs(origY).^2,        'b-',  'LineWidth', 1.2); hold on;
plot(t*1e9, abs(fiberY).^2,       'r--', 'LineWidth', 1.0);
plot(t*1e9, abs(compensatedY).^2, 'g-.', 'LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)');
title('Y-Pol: Intensity (Time Domain)');
legend('Original', 'Fiber out', 'DBP out'); grid on;

subplot(2,2,3);
specOrig  = fftshift(abs(fft(origX)).^2);
specFiber = fftshift(abs(fft(fiberX)).^2);
specDBP   = fftshift(abs(fft(compensatedX)).^2);
plot(f*1e-9, 10*log10(specOrig  + eps), 'b-',  'LineWidth', 1.2); hold on;
plot(f*1e-9, 10*log10(specFiber + eps), 'r--', 'LineWidth', 1.0);
plot(f*1e-9, 10*log10(specDBP   + eps), 'g-.', 'LineWidth', 1.0);
xlabel('Frequency (GHz)'); ylabel('Power (dB)');
title('X-Pol: Spectrum');
legend('Original', 'Fiber out', 'DBP out'); grid on; xlim([-200 200]);

subplot(2,2,4);
errX = abs(origX - compensatedX);
errY = abs(origY - compensatedY);
plot(t*1e9, errX, 'r-', 'LineWidth', 1); hold on;
plot(t*1e9, errY, 'b-', 'LineWidth', 1);
xlabel('Time (ns)'); ylabel('|Error|');
title(sprintf('DBP Compensation Error  (NMSE=%.2e)', nmseRoundTrip));
legend('X-Pol error', 'Y-Pol error'); grid on;

sgtitle(sprintf('Single-Span Round-Trip  (lSpan=%d km)  |  NMSE=%.2e', lSpan/1e3, nmseRoundTrip));

% ============================================================
% Figure 2: Multi-span Round-trip DBP
% Purpose: verify DBP works across multiple fiber spans.
% Expected: DBP-compensated signal ≈ original (NMSE small).
% Verification metric: NMSE (original vs multi-span DBP-compensated).
% ============================================================
fprintf('\n=== DP_DBP: Multi-Span Round-Trip Visualization ===\n');

[origX, origY] = readCmat('DBP_MultiSpan_Original.dat');
[compensatedX, compensatedY] = readCmat('DBP_MultiSpan_Compensated.dat');

nmseMulti = computeNMSE(origX, origY, compensatedX, compensatedY);
fprintf('  NMSE (multi-span DBP vs original): %.4e  (expected ~0)\n', nmseMulti);

nSpans = 3;
lSpan  = 1e3;  % [m] per span, match C++ test

figure('Name', 'DP_DBP: Multi-Span Round-Trip', 'Position', [150, 150, 1200, 800]);

subplot(2,2,1);
plot(t*1e9, abs(origX).^2,        'b-',  'LineWidth', 1.2); hold on;
plot(t*1e9, abs(compensatedX).^2, 'r--', 'LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)');
title('X-Pol: Intensity (Original vs DBP)');
legend('Original', 'DBP compensated'); grid on;

subplot(2,2,2);
plot(t*1e9, abs(origY).^2,        'b-',  'LineWidth', 1.2); hold on;
plot(t*1e9, abs(compensatedY).^2, 'r--', 'LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)');
title('Y-Pol: Intensity (Original vs DBP)');
legend('Original', 'DBP compensated'); grid on;

subplot(2,2,3);
specOrig = fftshift(abs(fft(origX)).^2);
specDBP  = fftshift(abs(fft(compensatedX)).^2);
plot(f*1e-9, 10*log10(specOrig + eps), 'b-',  'LineWidth', 1.2); hold on;
plot(f*1e-9, 10*log10(specDBP  + eps), 'r--','LineWidth', 1.0);
xlabel('Frequency (GHz)'); ylabel('Power (dB)');
title('X-Pol: Spectrum');
legend('Original', 'DBP compensated'); grid on; xlim([-200 200]);

subplot(2,2,4);
errX = abs(origX - compensatedX);
errY = abs(origY - compensatedY);
plot(t*1e9, errX, 'r-', 'LineWidth', 1); hold on;
plot(t*1e9, errY, 'b-', 'LineWidth', 1);
xlabel('Time (ns)'); ylabel('|Error|');
title(sprintf('DBP Compensation Error  (NMSE=%.2e)', nmseMulti));
legend('X-Pol error', 'Y-Pol error'); grid on;

sgtitle(sprintf('Multi-Span Round-Trip  (%d spans x %d m)  |  NMSE=%.2e', ...
    nSpans, lSpan, nmseMulti));

% ============================================================
% Local functions (must be at end of script per MATLAB rules)
% ============================================================

function [sigX, sigY] = readCmat(filename)
    % Read cmat (2 x Nt) from .dat file, return column vectors.
    data = load(filename);
    Ncols = size(data, 2);
    NtLocal = Ncols / 2;
    sigX = data(1, 1:2:end).' + 1j * data(1, 2:2:end).';
    sigY = data(2, 1:2:end).' + 1j * data(2, 2:2:end).';
end

function nmse = computeNMSE(refX, refY, estX, estY)
    % Normalized mean squared error between reference and estimated signals.
    num = sum(abs(refX - estX).^2 + abs(refY - estY).^2);
    den = sum(abs(refX).^2      + abs(refY).^2);
    nmse = num / den;
end
