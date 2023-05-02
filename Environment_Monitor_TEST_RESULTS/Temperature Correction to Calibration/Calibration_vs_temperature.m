clear;clc;close all;

% Data at 14.9 degreesC
V_in_15deg = [-10.0008 -8.9505 -7.9688 -6.9720 -5.9636 -4.9819 -4.023 -2.958 -1.9653 -0.976 0 0.999 2.067 3.0863 4.0128 5.0572 6.054 7.0283 8.0194 9.0743 10.0009];
Cnts_15deg = [32375.2 30695.4 29125 27531.3 25917.3 24347 22813.3 21109.9 19521.3 17939.5 16378.4 14780.2 13072.2 11441.6 9959.6 8289.3 6694.4 5136.5 3551.4 1868 483];

V_in_20deg = [-10 -9.0109 -7.9729 -6.9448 -6.018 -5.0035 -3.9924 -2.997 -2.0222 -1.0177 0 0.97486 1.9803 3.00295 4.01825 5.00425 6.0205 7.0083 8.023 9.0113 9.9997];
Cnts_20deg = [32375.3 30792.9 29133 27488.1 26006 24382.1 22765.3 21173.5 19613 18006.9 16379 14819.4 13211.1 11575.3 9951.4 8374.2 6748.7 5168.6 3545.7 1968.1 488.3];

V_in_31deg = [-9.9898 -8.9942 -7.9678 -6.9951 -6.0027 -5.0024 -4.0092 -2.9991 -1.9473 -1.014 0 1.0143 2.0661 3.0414 4.0301 5.0088 6.0027 7.0307 7.9919 8.9981 10.0006];
Cnts_31deg = [32361.9 30769.2 29127.3 27570.9 25983.5 24383.1 22794.8 21178.6 19495.7 18002.6 16381 14757.8 13075.1 11515 9933.3 8367.4 6777 5133.6 3595.8 1990.3 493.9];

V_in_46deg = [-9.9976 -8.0053 -5.9867 -4.0238 -2.02238 2.0225 4.04415 5.9978 8.0618 9.0537 9.9991];
Cnts_46deg = [32379.1 29191.7 25961.7 22820.7 19618.4 13146.8 9912 6786.4 3484.1 1903.8 505.3];

lw = 1;
figure('color','w');hold all;
p1 = plot(V_in_15deg, Cnts_15deg, 'o','linewidth',lw);
p2 = plot(V_in_20deg, Cnts_20deg, 'o','linewidth',lw);
p3 = plot(V_in_31deg, Cnts_31deg, 'o','linewidth',lw);
p4 = plot(V_in_46deg, Cnts_46deg, 'o','linewidth',lw);

xlabel('Analog Sensor Input Voltage (V)')
ylabel('ADC Reading (counts)')
box on;grid on;
set(gca,'LineWidth',1.5,'FontSize',12,'FontName','SansSerif')
title('Calibration of Sensor Voltage to ADC Counts','FontSize',11)
ax = gca;
ax.YAxis.Exponent = 0;
ytickformat('%.0f');
xlim([-1 1]*10.01)
legend([p1 p2 p3 p4],{'15^\circC','20^\circC','31^\circC','46^\circC'})

Cnts_pred_15 = -1599.61343100043 * V_in_15deg + 16379.03370099838; % Prediction using calibration from 20 degreesC
Cnts_pred_20 = -1599.61343100043 * V_in_20deg + 16379.03370099838; % Prediction using calibration from 20 degreesC
Cnts_pred_31 = -1599.61343100043 * V_in_31deg + 16379.03370099838; % Prediction using calibration from 20 degreesC
Cnts_pred_46 = -1599.61343100043 * V_in_46deg + 16379.03370099838; % Prediction using calibration from 20 degreesC

Cnts_fit_15deg = -1599.55407779787 * V_in_15deg + 16378.39899879612;
Cnts_fit_20deg = -1599.61343100043 * V_in_20deg + 16379.03370099838;
Cnts_fit_31deg = -1599.76622414015 * V_in_31deg + 16380.57999636840;
Cnts_fit_46deg = -1600.00880965492 * V_in_46deg + 16382.84052227336;

coeffs_slope = -[1599.55407779787 1599.61343100043 1599.76622414015 1600.00880965492];
coeffs_offset = [16378.39899879612 16379.03370099838 16380.57999636840 16382.84052227336];
T = [15 20 31 46];

figure('color','w');
subplot(1,2,1);hold all;
plot(V_in_15deg, Cnts_15deg - Cnts_pred_15,'s-')
plot(V_in_20deg, Cnts_20deg - Cnts_pred_20,'s-')
plot(V_in_31deg, Cnts_31deg - Cnts_pred_31,'s-')
plot(V_in_46deg, Cnts_46deg - Cnts_pred_46,'s-')
legend({'15^\circC','20^\circC','31^\circC','46^\circC'})
xlabel('Analog Sensor Input Voltage (V)')
ylabel('Deviation from Calibration (counts)')
xlim([-10 8.01])
ylim([-1 1]*8)
box on;grid on;
set(gca,'LineWidth',1.5,'FontSize',12)
title('Error when applying room tempearture calibration to all cases','FontSize',11)

subplot(1,2,2);hold all;
plot(V_in_15deg, Cnts_15deg - Cnts_fit_15deg,'s-')
plot(V_in_20deg, Cnts_20deg - Cnts_fit_20deg,'s-')
plot(V_in_31deg, Cnts_31deg - Cnts_fit_31deg,'s-')
plot(V_in_46deg, Cnts_46deg - Cnts_fit_46deg,'s-')
legend({'15^\circC','20^\circC','31^\circC','46^\circC'})
xlabel('Analog Sensor Input Voltage (V)')
ylabel('Deviation from Calibration (counts)')
xlim([-10 8.01])
ylim([-1 1]*8)
box on;grid on;
set(gca,'LineWidth',1.5,'FontSize',12)
title('Error when applying custom calibrations to each case','FontSize',11)

T_vec = linspace(10,60,100);

figure('color','w');
hold all;
plot(T, coeffs_offset,'ks','MarkerFaceColor','k')
plot(T_vec, 0.14383886517*T_vec + 16376.18581638417,'k-')
ax2 = gca;
ax2.YAxis.Exponent = 0;
ytickformat('%.0f');
box on;grid on;
set(gca,'LineWidth',1.5,'FontSize',12)
ylabel({'Offset required for correct calibration';'(counts)'})
xlabel('Ambient Temperature (^\circC)')

yyaxis right
hold all
plot(T, coeffs_slope,'o','color',[203,24,29]/255,'MarkerFaceColor',[203,24,29]/255)
plot(T_vec, -0.014746973204*T_vec - 1599.322720398640,'r-')
ylabel({'Slope required for correct calibration';'(counts / volt)'})
set(gca,'Ycolor',[203,24,29]/255)
