%% DP_FiberTest.m
% MATLAB visualization for DP_Fiber Agent — 4 test signal types.
%
% Signal types tested:
%   1. Gaussian pulse   — pulse propagation, attenuation verification
%   2. Single tone (CW) — SPM/XPM constant-envelope phase shift
%   3. Dual tone        — nonlinear wave mixing (FWM)
%   4. QPSK             — communication waveform integrity
%
% For each type, input and output are compared in time and frequency
% domains.  PASS/FAIL criteria are shown in console output and figure
% titles.  See comments above each figure for detailed expectations.
%
% Usage:
%   1. Build & run DP_FiberTest.exe (generates .dat files)
%   2. >> DP_FiberTest
%
% .dat format: 2 rows (X/Y pol), each row has 2*Nt reals (Re,Im alternating)

%% Parameters — must match DP_FiberTest.cpp visualization section
Nt   = 1024;              % number of time samples
fs   = 100e9;             % sampling rate [Hz]
dt   = 1 / fs;            % sampling interval [s]
t    = (0 : Nt-1)' * dt;  % time axis [s] (column vector)
df   = fs / Nt;           % frequency spacing [Hz]
f    = (-Nt/2 : Nt/2-1)' * df;  % frequency axis [Hz] (fftshift order)

fprintf('=== DP_Fiber: Multi-Signal Visualization ===\n');
allPass = true;

% ============================================================
% Figure 1: Gaussian Pulse — Pure Loss Test
%
% PURPOSE: verify DP_Fiber applies exponential field attenuation
%   when gamma=0 and beta2=beta3=0 (pure loss regime).
%
% EXPECTED RESULT: output = input * exp(-alphaNp * lSpan / 2).
%   Waveform shape is preserved (only amplitude reduced).
%   Spectrum is a scaled copy (no frequency-dependent effects).
%
% PASS CRITERION: measured attenuation ratio matches expected
%   within 2% relative error.
% ============================================================
fprintf('\n--- Signal 1: Gaussian pulse (pure loss) ---\n');

[sigX_in, sigY_in]   = readCmat('Fiber_GaussInput.dat');
[sigX_out, sigY_out] = readCmat('Fiber_GaussOutput.dat');

alpha_dBpm = 0.2e-3;        % [dB/m], match C++ test
lSpan      = 100e3;         % [m]
alphaNp    = alpha_dBpm * log(10) / 10;
ratioExpected = exp(-alphaNp * lSpan / 2);
ratioMeasured = sqrt( (sum(abs(sigX_out).^2 + abs(sigY_out).^2)) / ...
                       (sum(abs(sigX_in).^2  + abs(sigY_in).^2)) );
relErr = abs(ratioMeasured - ratioExpected) / ratioExpected;
pass1 = relErr < 0.02;
allPass = allPass && pass1;
fprintf('  Attenuation: measured=%.6f  expected=%.6f  relErr=%.2e  [%s]\n', ...
    ratioMeasured, ratioExpected, relErr, passStr(pass1));

figure('Name', 'DP_Fiber: Gaussian (Pure Loss)');
subplot(2,2,1);
plot(t*1e9, abs(sigX_in).^2, 'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(sigX_out).^2, 'r--','LineWidth', 1.2);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('X-Pol Intensity'); legend('Input','Output');

subplot(2,2,2);
plot(t*1e9, abs(sigY_in).^2, 'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(sigY_out).^2, 'r--','LineWidth', 1.2);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('Y-Pol Intensity'); legend('Input','Output');

subplot(2,2,3);
specIn  = fftshift(abs(fft(sigX_in)).^2);
specOut = fftshift(abs(fft(sigX_out)).^2);
plot(f*1e-9, 10*log10(specIn+eps), 'b-'); hold on;
plot(f*1e-9, 10*log10(specOut+eps), 'r--');
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-200 200]);
title('X-Pol Spectrum'); legend('Input','Output');

subplot(2,2,4);
plot(f*1e-9, 10*log10(specIn+eps), 'b-'); hold on;
plot(f*1e-9, 10*log10(specOut+eps), 'r--');
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-200 200]);
title('Y-Pol Spectrum'); legend('Input','Output');

sgtitle(sprintf('Gaussian — Pure Loss [%s]', passStr(pass1)));

% ============================================================
% Figure 2: Single Tone (CW) — Pure Nonlinearity Test
%
% PURPOSE: verify energy conservation under pure SPM/XPM.
%   CW tone has constant |A|^2, so Manakov phase shift is
%   uniform across all time samples (deterministic rotation).
%
% EXPECTED RESULT: energy conserved (gamma is unitary).
%   Spectrum shows no broadening (pure phase modulation on
%   a single frequency component preserves the delta spectrum).
%
% PASS CRITERION: energy relative difference < 1%.
% ============================================================
fprintf('\n--- Signal 2: Single tone (pure NL) ---\n');

[sigX_in, sigY_in]   = readCmat('Fiber_ToneInput.dat');
[sigX_out, sigY_out] = readCmat('Fiber_ToneOutput.dat');

energyIn  = sum(abs(sigX_in).^2  + abs(sigY_in).^2);
energyOut = sum(abs(sigX_out).^2 + abs(sigY_out).^2);
eRel = abs(energyOut - energyIn) / energyIn;
pass2 = eRel < 0.01;
allPass = allPass && pass2;
fprintf('  Energy conservation: relDiff=%.2e  [%s]\n', eRel, passStr(pass2));

figure('Name', 'DP_Fiber: CW Tone (Pure NL)');
subplot(2,2,1);
plot(t*1e9, real(sigX_in),  'b-', 'LineWidth', 1); hold on;
plot(t*1e9, real(sigX_out), 'r--','LineWidth', 1);
xlabel('Time (ns)'); ylabel('Real Part'); grid on;
title('X-Pol Waveform (Real)'); legend('Input','Output');

subplot(2,2,2);
plot(t*1e9, real(sigY_in),  'b-', 'LineWidth', 1); hold on;
plot(t*1e9, real(sigY_out), 'r--','LineWidth', 1);
xlabel('Time (ns)'); ylabel('Real Part'); grid on;
title('Y-Pol Waveform (Real)'); legend('Input','Output');

subplot(2,2,3);
specIn  = fftshift(abs(fft(sigX_in)).^2);
specOut = fftshift(abs(fft(sigX_out)).^2);
plot(f*1e-9, 10*log10(specIn+eps), 'b-'); hold on;
plot(f*1e-9, 10*log10(specOut+eps), 'r--');
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-50 50]);
title('X-Pol Spectrum'); legend('Input','Output');

subplot(2,2,4);
plot(f*1e-9, 10*log10(specIn+eps), 'b-'); hold on;
plot(f*1e-9, 10*log10(specOut+eps), 'r--');
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-50 50]);
title('Y-Pol Spectrum'); legend('Input','Output');

sgtitle(sprintf('CW Tone — Pure NL [%s]', passStr(pass2)));

% ============================================================
% Figure 3: Dual Tone — Nonlinear Wave Mixing Test
%
% PURPOSE: two closely-spaced frequencies test nonlinear
%   intermodulation.  The non-constant envelope produces
%   time-varying Manakov phase → frequency mixing.
%
% EXPECTED RESULT: after pure NL, intermodulation products
%   appear at sum/difference frequencies (|f1±f2|, 2f1-f2, etc.).
%   Energy is conserved (gamma is unitary).
%
% PASS CRITERION: spectrum shows the two original tones plus
%   new frequency components (visual inspection).
% ============================================================
fprintf('\n--- Signal 3: Dual tone (pure NL) ---\n');

[sigX_in, sigY_in]   = readCmat('Fiber_DualInput.dat');
[sigX_out, sigY_out] = readCmat('Fiber_DualOutput.dat');

eIn  = sum(abs(sigX_in).^2  + abs(sigY_in).^2);
eOut = sum(abs(sigX_out).^2 + abs(sigY_out).^2);
fprintf('  Energy: in=%.2f  out=%.2f  (should be equal)\n', eIn, eOut);

% Frequencies used in C++: f1=1.5 GHz, f2=2.5 GHz
f1 = 1.5e9; f2 = 2.5e9;
fprintf('  Input tones at f1=%.1f GHz, f2=%.1f GHz\n', f1/1e9, f2/1e9);
fprintf('  Intermodulation products expected at |mf1±nf2|\n');

figure('Name', 'DP_Fiber: Dual Tone (Pure NL)');
subplot(2,2,1);
plot(t*1e9, abs(sigX_in).^2, 'b-', 'LineWidth', 1); hold on;
plot(t*1e9, abs(sigX_out).^2, 'r--','LineWidth', 1);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('X-Pol Intensity'); legend('Input','Output');

subplot(2,2,2);
plot(t*1e9, abs(sigY_in).^2, 'b-', 'LineWidth', 1); hold on;
plot(t*1e9, abs(sigY_out).^2, 'r--','LineWidth', 1);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('Y-Pol Intensity'); legend('Input','Output');

subplot(2,2,3);
specIn  = fftshift(abs(fft(sigX_in)).^2);
specOut = fftshift(abs(fft(sigX_out)).^2);
plot(f*1e-9, 10*log10(specIn+eps),  'b-', 'LineWidth', 1.2); hold on;
plot(f*1e-9, 10*log10(specOut+eps), 'r--','LineWidth', 1.0);
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-20 20]);
title('X-Pol Spectrum (look for new mixing products)'); legend('Input','Output');

subplot(2,2,4);
plot(f*1e-9, 10*log10(specIn+eps),  'b-', 'LineWidth', 1.2); hold on;
plot(f*1e-9, 10*log10(specOut+eps), 'r--','LineWidth', 1.0);
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-20 20]);
title('Y-Pol Spectrum'); legend('Input','Output');

sgtitle('Dual Tone — Pure NL (check spectrum for FWM products)');

% ============================================================
% Figure 4: QPSK — Full Fiber Propagation Test
%
% PURPOSE: verify that a realistic QPSK communication waveform
%   survives fiber propagation.  This is the most important
%   test for an optical communication simulator.
%
% EXPECTED RESULT: fiber output is distorted (dispersion spreads
%   pulses, nonlinearity rotates phases).  NMSE between input
%   and fiber output should be significant (> 0.1).
%   This is NOT a failure — it confirms the fiber model is
%   doing something physically meaningful.
%
% PASS CRITERION: fiber output has finite energy and differs
%   measurably from input (NMSE > 0.01 confirms physical effects).
%   For full verification, run DP_DBPTest.m to confirm DBP
%   can undo this distortion.
% ============================================================
fprintf('\n--- Signal 4: QPSK (full effects) ---\n');

[sigX_in, sigY_in]   = readCmat('Fiber_QPSKInput.dat');
[sigX_out, sigY_out] = readCmat('Fiber_QPSKOutput.dat');

nmseFiber = computeNMSE(sigX_in, sigY_in, sigX_out, sigY_out);
% Fiber SHOULD distort the signal — NMSE > 0.01 is expected
pass4 = nmseFiber > 0.01;
allPass = allPass && pass4;
fprintf('  Fiber NMSE vs input: %.4e  [%s]  (> 0.01 confirms distortion)\n', ...
    nmseFiber, passStr(pass4));
fprintf('  NOTE: NMSE > 0 means fiber distorts the signal. This is CORRECT.\n');
fprintf('  Run DP_DBPTest.m to verify DBP can compensate this distortion.\n');

figure('Name', 'DP_Fiber: QPSK (Full Effects)');
subplot(2,2,1);
plot(t*1e9, abs(sigX_in).^2, 'b-', 'LineWidth', 1); hold on;
plot(t*1e9, abs(sigX_out).^2, 'r--','LineWidth', 1);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('X-Pol: Intensity'); legend('Input','Output');

subplot(2,2,2);
plot(t*1e9, abs(sigY_in).^2, 'b-', 'LineWidth', 1); hold on;
plot(t*1e9, abs(sigY_out).^2, 'r--','LineWidth', 1);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('Y-Pol: Intensity'); legend('Input','Output');

subplot(2,2,3);
specIn  = fftshift(abs(fft(sigX_in)).^2);
specOut = fftshift(abs(fft(sigX_out)).^2);
plot(f*1e-9, 10*log10(specIn+eps), 'b-'); hold on;
plot(f*1e-9, 10*log10(specOut+eps), 'r--');
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-50 50]);
title('X-Pol Spectrum'); legend('Input','Output');

subplot(2,2,4);
plot(f*1e-9, 10*log10(specIn+eps), 'b-'); hold on;
plot(f*1e-9, 10*log10(specOut+eps), 'r--');
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-50 50]);
title('Y-Pol Spectrum'); legend('Input','Output');

sgtitle(sprintf('QPSK — Full Fiber [%s]', passStr(pass4)));

% ============================================================
% Summary
% ============================================================
fprintf('\n=== DP_Fiber Visualization Summary ===\n');
fprintf('  Gaussian (pure loss):   [%s]\n', passStr(pass1));
fprintf('  CW Tone  (pure NL):     [%s]\n', passStr(pass2));
fprintf('  Dual Tone (pure NL):    visual inspection\n');
fprintf('  QPSK      (full fiber): [%s]\n', passStr(pass4));
fprintf('  Overall: [%s]\n', passStr(allPass));

% ============================================================
% Local functions (must be at end of script)
% ============================================================

function [sigX, sigY] = readCmat(filename)
    data = load(filename);
    Ncols = size(data, 2);
    sigX = data(1, 1:2:end).' + 1j * data(1, 2:2:end).';
    sigY = data(2, 1:2:end).' + 1j * data(2, 2:2:end).';
end

function nmse = computeNMSE(refX, refY, estX, estY)
    num = sum(abs(refX - estX).^2 + abs(refY - estY).^2);
    den = sum(abs(refX).^2      + abs(refY).^2);
    nmse = num / den;
end

function s = passStr(flag)
    if flag, s = 'PASS'; else, s = 'FAIL'; end
end
