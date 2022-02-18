clear; close all; clc;

%% ping pong delay prototype

Fs = 48000; % sampling rate

N = 200; % simulation length in samples
x = zeros(N,2); % Initialise input stereo array
x(1,1) = 1; % Left channel impulse function
x(5,2) = 0.4; % Right channel impulse function (delayed and attenuated)

% delay coefficients
dryWet = 0.5; % dry value, wet value = 1 - dryWet
feedback = 0.7; % feedback value
samples = Fs * 10;

% output vectors
leftOutput = zeros(samples, 1); % left channel out
rightOutput = zeros(samples, 1); % right channel out

%delay lines
bufferSize = Fs * 3; % buffer length in samples
bufferLeft = zeros(bufferSize, 1); % left delay line
bufferRight = zeros(bufferSize, 1); % right delay line

% %Tempo-Sync
% beatsPerMinute = 120;
% beatsPerSecond = beatsPerMinute/60;

% %find seconds per beat by taking the inverse
% secondsPerBeat = 1/beatsPerSecond;
% noteLength = 1; %0.5 = 1/8th, 0.75 = dotted 1/8th, 1 = 1/4, 2 = 1/2 
% delayInSamples = fix(noteLength * secondsPerBeat * Fs);

% read write pointers
delayInSeconds = 1;
delayInSamples = 20; %delayInSeconds * Fs;
delayReadIndex = 1; %read head (index to read from buffers)
delayWriteIndex = delayInSamples; %write head (index to write to buffers)


%% in the loop

for n = 1:N 
    
    % read samples from input
    leftChannel = x(n,1); % current input sample (left channel)
    rightChannel = x(n,2); % current input sample (right channel)
    
    % make buffers circular
    if (delayReadIndex > bufferSize)
        delayReadIndex = delayReadIndex - bufferSize;
    end
    if (delayWriteIndex > bufferSize)
        delayWriteIndex = delayWriteIndex - bufferSize;
    end

    %get the current output of the circular buffers
    delayOutLeft = bufferLeft(delayReadIndex);
    delayOutRight = bufferRight(delayReadIndex);

    %ping pong effect
    delayInLeft = rightChannel + feedback * delayOutRight; % Left delay block input
    delayInRight = leftChannel + feedback * delayOutLeft; % Right delay block input 

    %fill buffers with delayed signal
    bufferLeft(delayWriteIndex) = delayInLeft;
    bufferRight(delayWriteIndex) = delayInRight;

    %output sample calculation
    leftChannelOut = dryWet * leftChannel + (1-dryWet) * delayOutLeft;
    rightChannelOut = dryWet * rightChannel + (1-dryWet) * delayOutRight;

    %fill output channels
    leftOutput(n) = leftChannelOut;
    rightOutput(n) = rightChannelOut;

    %increment read and write
    delayReadIndex = delayReadIndex + 1;
    delayWriteIndex = delayWriteIndex + 1;
end

%% Delay Output

subplot(211)
stem(leftOutput)
axis([0 N 0 1])
title('MATLAB Render (Left Channel)', 'Fontsize', 14)
xlabel('samples (n)')
ylabel('amplitude')
grid on
subplot(212)
stem(rightOutput, 'm')
axis([0 N 0 1])
title('MATLAB Render (Right Channel)', 'Fontsize', 14)
xlabel('samples (n)')
ylabel('amplitude')
grid on

%% Feedback Plotting (C++ Render)

% [fb1, Fs] = audioread('renderFeedbackZeroPointOne.wav'); %Feedback value = 0.1
% [fb3, Fs] = audioread('renderFeedbackZeroPointThree.wav'); %Feedback value = 0.3
% [fb5, Fs] = audioread('renderFeedbackZeroPointFive.wav'); %Feedback value = 0.5
% [fb7, Fs] = audioread('renderFeedbackZeroPointSeven.wav'); %Feedback value = 0.7
% [fb9, Fs] = audioread('renderFeedbackZeroPointNine.wav'); %Feedback value = 0.9
% 
% %plotting for left channel
% stem(fb1(:,1))
% hold on
% stem(fb3(:,1))
% stem(fb5(:,1))
% stem(fb7(:,1))
% stem(fb9(:,1))
% axis([0 1000 0 0.6])
% title('C++ Render (Leff Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% grid on
% legend('feedback = 0.1', 'feedback = 0.3', 'feedback = 0.5', 'feedback = 0.7', 'feedback = 0.9')
% 
% %plotting for right channel
% stem(fb1(:,2))
% hold on
% stem(fb3(:,2))
% stem(fb5(:,2))
% stem(fb7(:,2))
% stem(fb9(:,2))
% axis([0 1000 0 0.6])
% title('C++ Render (Right Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% grid on
% legend('feedback = 0.1', 'feedback = 0.3', 'feedback = 0.5', 'feedback = 0.7', 'feedback = 0.9')


%% Delay Time Plotting (C++ Render)

% [zeroPointOneSecDelay, Fs] = audioread('CPPDelayTimePointOneFeedbackPointFive.wav');
% [zeroPointFiveSecDelay, Fs] = audioread('CPPDelayTimePointFiveFeedbackPointFive.wav');
% [oneSecDelay, Fs] = audioread('CPPDelayTimeOneFeedbackPointFive.wav');
% [onePointFiveSecDelay, Fs] = audioread('CPPDelayTimeOnePointFiveFeedbackPointFive.wav');
% [twoSecDelay, Fs] = audioread('CPPDelayTimeTwoFeedbackPointFive.wav');

% subplot(211)
% stem(zeroPointFiveSecDelay(:,1))
% title('C++ Render (Left Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 0.5 (seconds)')
% grid on
% subplot(212)
% stem(zeroPointFiveSecDelay(:,2), 'r')
% title('C++ Render (Right Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 0.5 (seconds)')
% grid on

% subplot(211)
% stem(zeroPointOneSecDelay(:,1))
% title('C++ Render (Left Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 0.1 (seconds)')
% grid on
% subplot(212)
% stem(zeroPointOneSecDelay(:,2), 'r')
% title('C++ Render (Right Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 0.1 (seconds)')
% grid on

% subplot(211)
% stem(onePointFiveSecDelay(:,1))
% title('C++ Render (Left Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 1.5 (seconds)')
% grid on
% subplot(212)
% stem(onePointFiveSecDelay(:,2), 'r')
% title('C++ Render (Right Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 1.5 (seconds)')
% grid on

% subplot(211)
% stem(oneSecDelay(:,1))
% title('C++ Render (Left Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 1.0 (seconds)')
% grid on
% subplot(212)
% stem(oneSecDelay(:,2), 'r')
% title('C++ Render (Right Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 1.0 (seconds)')
% grid on

% subplot(211)
% stem(twoSecDelay(:,1))
% title('C++ Render (Left Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 2.0 (seconds)')
% grid on
% subplot(212)
% stem(twoSecDelay(:,2), 'r')
% title('C++ Render (Right Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 2.0 (seconds)')
% grid on
% stem(zeroPointOneSecDelay(:,1))
% hold on
% stem(zeroPointFiveSecDelay(:,1))
% stem(oneSecDelay(:,1))
% stem(onePointFiveSecDelay(:,1))
% stem(twoSecDelay(:,1))
% title('C++ Render (Left Channel)','Fontsize', 14)
% xlabel('Samples (n)')
% ylabel('Amplitude')
% legend('delay time = 0.1 (seconds)', 'delay time = 0.5 (seconds)', 'delay time = 1.0 (seconds)', 'delay time = 1.5 (seconds)', 'delay time = 2.0 (seconds)')
%  grid on

%% Tempo Synced Delay Plotting (C++ Render)

[hundredBPMnoteLengthOne, Fs] = audioread('100bpmtest_note_length_one.wav');
[hundredBPMnoteLengthPointFive, Fs] = audioread('100bpmtest_note_length_point_five.wav');
[hundredBPMnoteLengthTwo, Fs] = audioread('100bpmtest_note_length_two.wav');
[hundredTwentyBPMnoteLengthTwo, Fs] = audioread('120bpmtest_note_length_two.wav');

figure(1)
subplot(211)
stem(hundredBPMnoteLengthPointFive(:,1))
title('1/8th note delay (Right Channel)', 'Fontsize', 14)
legend('100bpm')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 15000 ])
subplot(212)
stem(hundredBPMnoteLengthPointFive(:,2), 'm')
title('1/8th note delay (Right Channel)', 'Fontsize', 14)
legend('100bpm')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 15000 ])

figure(2)
subplot(211)
stem(hundredBPMnoteLengthOne(:,1))
title('1/4 note delay (Right Channel)', 'Fontsize', 14)
legend('100bpm')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 30000 ])
subplot(212)
stem(hundredBPMnoteLengthOne(:,2), 'm')
title('1/4 note delay (Right Channel)', 'Fontsize', 14)
legend('100bpm')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 30000 ])

figure(3)
subplot(211)
stem(hundredBPMnoteLengthTwo(:,1))
title('1/2 note delay (Right Channel)', 'Fontsize', 14)
legend('100bpm')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 200000 ])
subplot(212)
stem(hundredBPMnoteLengthTwo(:,2), 'm')
title('1/2 note delay (Right Channel)', 'Fontsize', 14)
legend('100bpm')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 200000 ])

figure(4)
stem(hundredBPMnoteLengthPointFive(:,2))
hold on
stem(hundredBPMnoteLengthOne(:,2))
stem(hundredBPMnoteLengthTwo(:,2))
title('1/8th, 1/4 and 1/2 note delay 100bpm (Right Channel)', 'Fontsize', 14)
legend('1/8', '1/4', '1/2')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 200000 ])

figure(5)
subplot(211)
stem(hundredTwentyBPMnoteLengthTwo(:,1))
title('1/2 note delay (Right Channel)', 'Fontsize', 14)
legend('120bpm')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 200000 ])
subplot(212)
stem(hundredTwentyBPMnoteLengthTwo(:,2), 'm')
title('1/2 note delay (Right Channel)', 'Fontsize', 14)
legend('120bpm')
xlabel('samples (n)')
ylabel('amplitude')
grid on
xlim([ 0 200000 ])