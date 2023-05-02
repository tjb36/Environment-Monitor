close all;clear;clc;

%   //                                                                ADS1015  ADS1115
%   //                                                                -------  -------
%   // ads.setGain(GAIN_TWOTHIRDS);  // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
%   // ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
%   // ads.setGain(GAIN_TWO);        // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
%   // ads.setGain(GAIN_FOUR);       // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
%   // ads.setGain(GAIN_EIGHT);      // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
%   // ads.setGain(GAIN_SIXTEEN);    // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

% MEASURED: GAIN = 2/3 , SINGLE ENDED , VCC = 5.00 V
V_Gain23 = [0 6.4e-3 28e-3 162e-3 290e-3 582e-3 1.047 1.562 2.062 2.568 4.005 4.5 4.944 5 5.063 5.098 5.157 5.199 5.286];
Counts_Gain23 = [-5 31 144 859 1542 3100 5581 8328 10993 13692 21344 23985 26348 26650 26988 27177 27487 27708 28173];

% MEASURED: GAIN = 2 , SINGLE ENDED , VCC = 5.00 V
V_Gain2 = [0 41e-3 57e-3 92e-3 136e-3 391e-3 451e-3 839e-3 1.034 1.431 1.773 1.952 1.99 2.01 2.051 2.074 2.1 2.2 2.3];
Counts_Gain2 = [-15 643 906 1457 2160 6239 7205 13400 16535 22894 28356 31223 31828 32141 32767 32767 32767 32767 32767];

% PREDICTED: GAIN = 1 , SINGEL ENDED , VCC = 5.00 V
V_Gain1 = linspace(0,5,10000);
Counts_Gain1 = V_Gain1 * 8000;
Counts_Gain1(Counts_Gain1>2^(15))=2^(15);


figure('Position',[277.8000  442.6000  770.2000  415.4000],'color','w'); hold all;
p(1) = plot(V_Gain2 , Counts_Gain2 , '.-');
p(2) = plot(V_Gain1 , Counts_Gain1 , '-' , 'LineWidth',1.5);
p(3) = plot(V_Gain23 , Counts_Gain23 , '.-');
l(1) = line([1 1]*5,[0 35000],'LineStyle','--','color','k');
l(2) = line([1 1]*5.3,[0 35000],'LineStyle','-.','color',[0.6 0.6 0.6]);
l(3) = line([0 6.5],[1 1]*2^15,'LineStyle','--','color','r');
l(4) = plot( linspace(5.286,6.1466,100) , linspace(5.286,6.1466,100)*5331 ,'LineStyle','-.','color','b');
xlabel('Analog Input Voltage (V)')
ylabel('ADC Reading (counts)')
box on
ylim([-100 35000])
xlim([0 6.5])
set(gca,'LineWidth',1.5)

annotation(gcf,'textbox',...
    [0.262613031865563 0.757679115941034 0.22757142857143 0.100952380952384],...
    'String',{'16000 counts / V','(62.5 \muV / count)'},...
    'LineStyle','none',...
    'FontSize',9,...
    'FitBoxToText','off');
annotation(gcf,'textbox',...
    [0.390095132989577 0.445638856409198 0.22757142857143 0.100952380952384],...
    'String',{'5331 counts / V','(187.6 \muV / count)'},...
    'LineStyle','none',...
    'FontSize',9,...
    'FitBoxToText','off');
annotation(gcf,'textbox',...
    [0.408270881598252 0.704670010539717 0.227571428571432 0.100952380952384],...
    'String',{'8000 counts / V','(125 \muV / count)'},...
    'LineStyle','none',...
    'FontSize',9,...
    'FitBoxToText','off');

legend('Gain = 2','Gain = 1','Gain = 2/3','V_s = 5.0 V','V_m_a_x = V_s + 0.3 V','Max counts = 2^1^5 = 32768','location','northeastoutside')
title('ADS1115 Behvaiour in Single-Ended Mode','FontSize',11)