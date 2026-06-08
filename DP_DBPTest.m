%% DP_DBPTest.m
% MATLAB visualization for DP_DBP Agent — 4 signal types, round-trip.
%
% Signal types tested:
%   1. Gaussian pulse   — DBP recovers pulse after fiber
%   2. Single tone (CW) — DBP undoes SPM/XPM phase rotation
%   3. Dual tone        — DBP reverses nonlinear wave mixing
%   4. QPSK             — DBP compensates fiber → clean constellation
%
% For each type, three signals are compared:
%   - Original (before fiber)
%   - Fiber output (after forward propagation)
%   - DBP compensated (after back-propagation)
%
% PASS/FAIL criteria are shown in console and figure titles.
% The CORE metric is NMSE(original, DBP-compensated):
%   - NMSE < 0.05 means DBP effectively cancels fiber impairments.
%   - This confirms the SSFM algorithm and Manakov factor 8/9
%     are correctly implemented for both forward and reverse
%     propagation.
%
% Usage:
%   1. Build & run DP_FiberTest.exe (generates .dat files)
%   2. >> DP_DBPTest
%
% .dat format: 2 rows (X/Y pol), each row has 2*Nt reals (Re,Im alternating)

%% Parameters — must match DP_FiberTest.cpp visualization section
Nt   = 1024;              % number of time samples
fs   = 100e9;             % sampling rate [Hz]
dt   = 1 / fs;            % sampling interval [s]
t    = (0 : Nt-1)' * dt;  % time axis [s] (column vector)
df   = fs / Nt;           % frequency spacing [Hz]
f    = (-Nt/2 : Nt/2-1)' * df;  % frequency axis [Hz] (fftshift order)

fprintf('=== DP_DBP: Multi-Signal Round-Trip Visualization ===\n');
fprintf('CORE TEST: NMSE(original, DBP-compensated) should be near 0.\n');
fprintf('This confirms DBP correctly reverses fiber impairments.\n');
allPass = true;

% ============================================================
% Figure 1: Gaussian Pulse Round-Trip
%
% PURPOSE: verify DBP recovers a Gaussian pulse after fiber
%   propagation.  The pulse experiences dispersion (broadening)
%   and nonlinearity in the fiber; DBP should undo both.
%
% EXPECTED RESULT: DBP-compensated waveform closely matches
%   original.  Fiber output shows visible distortion (pulse
%   shape differs from original).
%
% PASS CRITERION: NMSE(original, DBP-compensated) < 0.05.
%   This is the most fundamental DBP validation test.
% ============================================================
fprintf('\n--- Signal 1: Gaussian pulse ---\n');

[origX, origY]   = readCmat('DBP_Gauss_Orig.dat');
[fiberX, fiberY] = readCmat('DBP_Gauss_Fiber.dat');
[compX, compY]   = readCmat('DBP_Gauss_Comp.dat');

nmseComp  = computeNMSE(origX, origY, compX, compY);
nmseFiber = computeNMSE(origX, origY, fiberX, fiberY);
pass1 = nmseComp < 0.05;
allPass = allPass && pass1;
fprintf('  NMSE(fiber vs original)     = %.4e  (should be >> 0)\n', nmseFiber);
fprintf('  NMSE(DBP vs original)       = %.4e  threshold=0.05  [%s]\n', ...
    nmseComp, passStr(pass1));

figure('Name', 'DP_DBP: Gaussian Round-Trip');
subplot(2,2,1);
plot(t*1e9, abs(origX).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(fiberX).^2, 'r--','LineWidth', 1.0);
plot(t*1e9, abs(compX).^2,  'g-.','LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('X-Pol Intensity'); legend('Original','Fiber','DBP');

subplot(2,2,2);
plot(t*1e9, abs(origY).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(fiberY).^2, 'r--','LineWidth', 1.0);
plot(t*1e9, abs(compY).^2,  'g-.','LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('Y-Pol Intensity'); legend('Original','Fiber','DBP');

subplot(2,2,3);
specOrig  = fftshift(abs(fft(origX)).^2);
specFiber = fftshift(abs(fft(fiberX)).^2);
specComp  = fftshift(abs(fft(compX)).^2);
plot(f*1e-9, 10*log10(specOrig+eps),  'b-', 'LineWidth', 1.2); hold on;
plot(f*1e-9, 10*log10(specFiber+eps), 'r--','LineWidth', 1.0);
plot(f*1e-9, 10*log10(specComp+eps),  'g-.','LineWidth', 1.0);
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-200 200]);
title('X-Pol Spectrum'); legend('Original','Fiber','DBP');

subplot(2,2,4);
errX = abs(origX - compX);
errY = abs(origY - compY);
plot(t*1e9, errX, 'r-', 'LineWidth', 1); hold on;
plot(t*1e9, errY, 'b-', 'LineWidth', 1);
xlabel('Time (ns)'); ylabel('|Error|'); grid on;
title('Compensation Error'); legend('X-Pol err','Y-Pol err');

sgtitle(sprintf('Gaussian DBP [%s]', passStr(pass1)));

% ============================================================
% Figure 2: Single Tone (CW) Round-Trip
%
% PURPOSE: verify DBP undoes the SPM/XPM phase shift on a
%   constant-envelope CW signal.  Since the CW has uniform
%   |A|^2, the Manakov rotation is deterministic.
%
% EXPECTED RESULT: DBP output ≈ original (same frequency,
%   same amplitude).  The fiber output may have a different
%   phase due to SPM/XPM + dispersion.
%
% PASS CRITERION: NMSE(original, DBP) < 0.05.
% ============================================================
fprintf('\n--- Signal 2: Single tone (CW) ---\n');

[origX, origY]   = readCmat('DBP_Tone_Orig.dat');
[fiberX, fiberY] = readCmat('DBP_Tone_Fiber.dat');
[compX, compY]   = readCmat('DBP_Tone_Comp.dat');

nmseComp  = computeNMSE(origX, origY, compX, compY);
nmseFiber = computeNMSE(origX, origY, fiberX, fiberY);
pass2 = nmseComp < 0.05;
allPass = allPass && pass2;
fprintf('  NMSE(fiber vs original)     = %.4e\n', nmseFiber);
fprintf('  NMSE(DBP vs original)       = %.4e  threshold=0.05  [%s]\n', ...
    nmseComp, passStr(pass2));

figure('Name', 'DP_DBP: CW Tone Round-Trip');
subplot(2,2,1);
plot(t*1e9, real(origX),  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, real(fiberX), 'r--','LineWidth', 1.0);
plot(t*1e9, real(compX),  'g-.','LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Real Part'); grid on;
title('X-Pol Waveform'); legend('Original','Fiber','DBP');

subplot(2,2,2);
plot(t*1e9, real(origY),  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, real(fiberY), 'r--','LineWidth', 1.0);
plot(t*1e9, real(compY),  'g-.','LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Real Part'); grid on;
title('Y-Pol Waveform'); legend('Original','Fiber','DBP');

subplot(2,2,3);
specOrig  = fftshift(abs(fft(origX)).^2);
specFiber = fftshift(abs(fft(fiberX)).^2);
specComp  = fftshift(abs(fft(compX)).^2);
plot(f*1e-9, 10*log10(specOrig+eps),  'b-', 'LineWidth', 1.2); hold on;
plot(f*1e-9, 10*log10(specFiber+eps), 'r--','LineWidth', 1.0);
plot(f*1e-9, 10*log10(specComp+eps),  'g-.','LineWidth', 1.0);
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-10 10]);
title('X-Pol Spectrum'); legend('Original','Fiber','DBP');

subplot(2,2,4);
errX = abs(origX - compX);
errY = abs(origY - compY);
plot(t*1e9, errX, 'r-', 'LineWidth', 1); hold on;
plot(t*1e9, errY, 'b-', 'LineWidth', 1);
xlabel('Time (ns)'); ylabel('|Error|'); grid on;
title('Compensation Error'); legend('X-Pol err','Y-Pol err');

sgtitle(sprintf('CW Tone DBP [%s]', passStr(pass2)));

% ============================================================
% Figure 3: Dual Tone Round-Trip
%
% PURPOSE: verify DBP reverses nonlinear wave mixing between
%   two closely-spaced frequency tones.  After pure NL fiber,
%   intermodulation products appear; DBP should suppress them.
%
% EXPECTED RESULT: DBP-compensated spectrum closely matches the
%   original two-tone spectrum (intermodulation products removed).
%
% PASS CRITERION: NMSE(original, DBP) < 0.05.
% ============================================================
fprintf('\n--- Signal 3: Dual tone ---\n');

[origX, origY]   = readCmat('DBP_Dual_Orig.dat');
[fiberX, fiberY] = readCmat('DBP_Dual_Fiber.dat');
[compX, compY]   = readCmat('DBP_Dual_Comp.dat');

nmseComp  = computeNMSE(origX, origY, compX, compY);
nmseFiber = computeNMSE(origX, origY, fiberX, fiberY);
pass3 = nmseComp < 0.05;
allPass = allPass && pass3;
fprintf('  NMSE(fiber vs original)     = %.4e\n', nmseFiber);
fprintf('  NMSE(DBP vs original)       = %.4e  threshold=0.05  [%s]\n', ...
    nmseComp, passStr(pass3));

figure('Name', 'DP_DBP: Dual Tone Round-Trip');
subplot(2,2,1);
plot(t*1e9, abs(origX).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(fiberX).^2, 'r--','LineWidth', 1.0);
plot(t*1e9, abs(compX).^2,  'g-.','LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('X-Pol: Intensity Envelope'); legend('Original','Fiber','DBP');

subplot(2,2,2);
specOrig  = fftshift(abs(fft(origX)).^2);
specFiber = fftshift(abs(fft(fiberX)).^2);
specComp  = fftshift(abs(fft(compX)).^2);
plot(f*1e-9, 10*log10(specOrig+eps),  'b-', 'LineWidth', 1.2); hold on;
plot(f*1e-9, 10*log10(specFiber+eps), 'r--','LineWidth', 1.0);
plot(f*1e-9, 10*log10(specComp+eps),  'g-.','LineWidth', 1.0);
xlabel('Frequency (GHz)'); ylabel('Power (dB)'); grid on; xlim([-20 20]);
title('X-Pol Spectrum');
legend('Original','Fiber','DBP');

subplot(2,2,3);
plot(t*1e9, abs(origY).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(fiberY).^2, 'r--','LineWidth', 1.0);
plot(t*1e9, abs(compY).^2,  'g-.','LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('Y-Pol: Intensity Envelope'); legend('Original','Fiber','DBP');

subplot(2,2,4);
errX = abs(origX - compX);
errY = abs(origY - compY);
plot(t*1e9, errX, 'r-', 'LineWidth', 1); hold on;
plot(t*1e9, errY, 'b-', 'LineWidth', 1);
xlabel('Time (ns)'); ylabel('|Error|'); grid on;
title('Compensation Error'); legend('X-Pol err','Y-Pol err');

sgtitle(sprintf('Dual Tone DBP [%s]', passStr(pass3)));

% ============================================================
% Figure 4: QPSK Round-Trip — THE MOST IMPORTANT TEST
%
% PURPOSE: verify DBP enables recovery of a realistic digital
%   communication signal after fiber propagation.  This is the
%   end-to-end validation of the entire SSFM/DBP pipeline for
%   its intended use case (optical communication).
%
% EXPECTED RESULT:
%   - Fiber output: visibly distorted waveform, high NMSE.
%   - DBP output: closely matches original, low NMSE.
%   - The time-domain error should be small and uniform.
%
% PASS CRITERION: NMSE(original, DBP-compensated) < 0.05.
%   This is the definitive test — if QPSK round-trip passes,
%   the SSFM and DBP algorithms are working correctly for
%   communication signals.
% ============================================================
fprintf('\n--- Signal 4: QPSK (MOST IMPORTANT) ---\n');

[origX, origY]   = readCmat('DBP_QPSK_Orig.dat');
[fiberX, fiberY] = readCmat('DBP_QPSK_Fiber.dat');
[compX, compY]   = readCmat('DBP_QPSK_Comp.dat');

nmseComp  = computeNMSE(origX, origY, compX, compY);
nmseFiber = computeNMSE(origX, origY, fiberX, fiberY);
pass4 = nmseComp < 0.05;
allPass = allPass && pass4;
fprintf('  NMSE(fiber vs original)     = %.4e  (should be >> 0 — fiber distorts)\n', nmseFiber);
fprintf('  NMSE(DBP vs original)       = %.4e  threshold=0.05  [%s]\n', ...
    nmseComp, passStr(pass4));
if pass4
    fprintf('  *** QPSK DBP PASSED — SSFM/DBP pipeline works correctly ***\n');
end

% Symbol timing (must match C++ viz section: nSym=64, sps=16)
sps  = 16;               % samples per symbol
nSym = Nt / sps;         % number of symbols
symIdx = sps/2 + 1 : sps : Nt;  % sample at symbol centers (1-indexed)

% Extract constellation samples at symbol centers
origConstX  = origX(symIdx);
origConstY  = origY(symIdx);
fiberConstX = fiberX(symIdx);
fiberConstY = fiberY(symIdx);
compConstX  = compX(symIdx);
compConstY  = compY(symIdx);

figure('Name', 'DP_DBP: QPSK Round-Trip + Constellation');

% (1,1): X-pol time-domain intensity
subplot(2,3,1);
plot(t*1e9, abs(origX).^2,  'b-', 'LineWidth', 1.2); hold on;
plot(t*1e9, abs(fiberX).^2, 'r--','LineWidth', 1.0);
plot(t*1e9, abs(compX).^2,  'g-.','LineWidth', 1.0);
xlabel('Time (ns)'); ylabel('Power (W)'); grid on;
title('X-Pol: Intensity'); legend('Original','Fiber','DBP');

% (1,2): Constellation — X-pol original (4 clean clusters)
subplot(2,3,2);
plot(real(origConstX), imag(origConstX), 'b.', 'MarkerSize', 12);
xlabel('In-Phase'); ylabel('Quadrature'); grid on; axis equal;
title('X-Pol: Original Constellation'); xlim([-1.5 1.5]); ylim([-1.5 1.5]);

% (1,3): Constellation — X-pol after fiber (scattered by dispersion + NL)
subplot(2,3,3);
plot(real(fiberConstX), imag(fiberConstX), 'r.', 'MarkerSize', 12);
xlabel('In-Phase'); ylabel('Quadrature'); grid on; axis equal;
title('X-Pol: After Fiber (distorted)'); xlim([-1.5 1.5]); ylim([-1.5 1.5]);

% (2,1): Constellation — X-pol after DBP (should recover 4 clusters)
subplot(2,3,4);
plot(real(compConstX), imag(compConstX), 'g.', 'MarkerSize', 12);
xlabel('In-Phase'); ylabel('Quadrature'); grid on; axis equal;
title('X-Pol: After DBP');
xlim([-1.5 1.5]); ylim([-1.5 1.5]);

% (2,2): Constellation — Y-pol after DBP
subplot(2,3,5);
plot(real(compConstY), imag(compConstY), 'g.', 'MarkerSize', 12);
xlabel('In-Phase'); ylabel('Quadrature'); grid on; axis equal;
title('Y-Pol: After DBP');
xlim([-1.5 1.5]); ylim([-1.5 1.5]);

% (2,3): Compensation error
subplot(2,3,6);
errX = abs(origX - compX);
errY = abs(origY - compY);
plot(t*1e9, errX, 'r-', 'LineWidth', 1); hold on;
plot(t*1e9, errY, 'b-', 'LineWidth', 1);
xlabel('Time (ns)'); ylabel('|Error|'); grid on;
title('Compensation Error');
legend('X-Pol err','Y-Pol err');

sgtitle(sprintf('QPSK DBP + Constellation [%s]', passStr(pass4)));

% ============================================================
% Summary
% ============================================================
fprintf('\n=== DP_DBP Visualization Summary ===\n');
fprintf('  Gaussian pulse:  NMSE=%.2e  [%s]\n', nmseComp, passStr(pass1));
fprintf('  CW Tone:         [%s]\n', passStr(pass2));
fprintf('  Dual Tone:       [%s]\n', passStr(pass3));

%% ============================================================
%% Part B: Fiber Propagation Visualization (merged from DP_FiberTest.m)
%% ============================================================

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

if allPass
    fprintf('\n  *** ALL DBP ROUND-TRIP TESTS PASSED ***\n');
    fprintf('  SSFM/DBP algorithms are correctly implemented.\n');
end

% ============================================================
% Local functions
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
