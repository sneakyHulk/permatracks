figure

x = 0:0.1:60;
y = 4.*cos(x)./(x+2);

m = 2;
n = 10;
t = tiledlayout(m,n,'TileSpacing','compact');

for i = 1:m
    bgAx(i) = axes(t,'XTick',[],'YTick',[],'Box','off');
    bgAx(i).Layout.TileSpan = [1 n];
    bgAx(i).Layout.Tile = (i - 1) * n + 1;

    for j = 1:n
        ax(i, j) = axes(t);
        ax(i, j).Layout.Tile = (i - 1) * n + j;
        plot(ax(i, j), x, y)
        ax(i, j).YAxis.Visible = 'off';
        ax(i, j).Box = 'off';
        xlim(ax(i, j),[0 15])
        xlabel(ax(i, j), num2str(j))
    end
    ax(i, 1).YAxis.Visible = 'on';

    linkaxes(ax(i, :), 'y')
end

nexttile(5)
hold on;
plot([1 2], [1 2])

figure
for i = 1:30


    
end