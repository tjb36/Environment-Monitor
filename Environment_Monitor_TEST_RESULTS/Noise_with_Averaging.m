
figure('color','w');
plot([1 10 30 100 200],[2.4884 0.7341 0.4574 0.291 0.1879]*4.38,'ko-','MarkerFaceColor','k','LineWidth',2)
xlabel('Number of readings used for averaging')
ylabel('RMS Noise (nT)')
box on;grid on;
set(gca,'LineWidth',1.5,'FontSize',12)
title('Reducing Noise by Averaging over Samples','FontSize',11)
