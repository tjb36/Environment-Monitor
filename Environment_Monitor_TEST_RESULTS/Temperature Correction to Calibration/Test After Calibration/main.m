clear;clc;close all;

load('logger_data.mat')

t_hours_start = 2051+0.24;

figure('color','w');
subplot(3,1,1)
plot(logger_TT.time_hours-t_hours_start, logger_TT.Temperature)
xlim([0 1.3])
ylabel('Ambient Temperature (^\circC)')
box on;grid on;
set(gca,'LineWidth',1,'FontSize',12,'FontName','Segoe UI Semilight','Ycolor',[1 1 1]*77/255,'Xcolor',[1 1 1]*77/255)
set(gca,'XTickLabel',{})
ylim([0 60])

subplot(3,1,2);hold all;
plot(logger_TT.time_hours-t_hours_start, (logger_TT.BX-5.00372)*1000,'color',[229,245,224]/255)
plot(logger_TT.time_hours-t_hours_start, (logger_TT.BX+0.0001-5.00372)*1000,'color',[65,171,93]/255)
plot(fluke_TT.time_hours-1-t_hours_start, (fluke_TT.fluke_voltage-5.00372 )*1000,'color','k')
xlim([0 1.3])
ylabel('Voltage (mV)')
box on;grid on;
set(gca,'LineWidth',1,'FontSize',12,'FontName','Segoe UI Semilight','Ycolor',[1 1 1]*77/255,'Xcolor',[1 1 1]*77/255)
legend('Original CH1 signal','CH1 signal with -\muV shift','True signal')
set(gca,'XTickLabel',{})

subplot(3,1,3);hold all;
hold all;
p2(1) = plot(logger_TT.time_hours-t_hours_start, (logger_TT.BZ)*1000, 'color',[222,235,247]/255);
p2(2) = plot(logger_TT.time_hours-t_hours_start, (logger_TT.BY)*1000, 'color',[254,224,210]/255);
p2(3) = plot(logger_TT.time_hours-t_hours_start, (logger_TT.BY-0.00016)*1000, 'color',[66,146,198]/255);
p2(4) = plot(logger_TT.time_hours-t_hours_start, (logger_TT.BZ-0.00056)*1000, 'color',[239,59,44]/255);
xlim([0 1.3])
xlabel('Time (hours)')
ylabel('Voltage (mV)')
box on;grid on;
set(gca,'LineWidth',1,'FontSize',12,'FontName','Segoe UI Semilight','Ycolor',[1 1 1]*77/255,'Xcolor',[1 1 1]*77/255)
legend('Original CH2 signal','Original CH3 signal','CH2 signal with -160\muV shift','CH3 signal with -560\muV shift')
