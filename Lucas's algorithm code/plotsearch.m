function plotsearch(map,visited,steps)
%PLOTSEARCH Draw the map showing both the explored cells and the final path.
%   plotsearch(m,v,s) after [m,v,s] = bfs('map_8.txt',[14,1],[1,18])
%
%   Walls          - red
%   Explored cells - light blue (every cell the algorithm looked at)
%   Shortest path  - yellow, numbered in step order, drawn on top
%
%   Because the path squares are inset, the blue shows as a halo around
%   them, so the path is visible as a subset of the explored region.

if nargin < 3
    steps = [];
end
if nargin < 2
    visited = [];
end

wallcolour     = [1.00 0.00 0.00];
exploredcolour = [0.68 0.85 0.95];
pathcolour     = [1.00 1.00 0.00];

[maprows,mapcols] = size(map);

[X,Y] = meshgrid(1:20,1:20);
plot(X,Y,'k'); hold on
plot(Y,X,'k');
axis([0, mapcols+1, 0, maprows+1]);
axis off

% Force a light background so the figure prints the same under MATLAB's
% dark theme, and so unexplored open cells read as empty white space.
set(gcf,'Color','w');
set(gca,'Color','w');

% Walls first, so everything else draws over them.
for i = 1:maprows
    for j = 1:mapcols
        if (map(i,j) == 1)
            placecell(maprows+1-i, j, wallcolour);
        end
    end
end

% Every cell the search looked at, numbered in the order it was explored.
% Cells that end up on the path are left unnumbered here, because the path
% square is drawn over them and carries its own step number instead.
if isempty(steps)
    onpath = false(size(visited,1),1);
else
    onpath = ismember(visited, steps, 'rows');
end

for k = 1:size(visited,1)
    placecell(maprows+1-visited(k,1), visited(k,2), exploredcolour);
end
for k = 1:size(visited,1)
    if ~onpath(k)
        placeorder(maprows+1-visited(k,1), visited(k,2), k);
    end
end

% The final path last, so it sits on top of the explored cells.
for k = 1:size(steps,1)
    placestep(maprows+1-steps(k,1), steps(k,2), k, pathcolour);
end

% Row indices down both sides.
for i = 1:maprows
    c = sprintf('%d',i);
    text(0.5,maprows-i+1+0.5,c,'FontSize',8,'Color','k');
    text(mapcols+1+0.5,maprows-i+1+0.5,c,'FontSize',8,'Color','k');
end
% Column indices along the top and bottom.
for i = 1:mapcols
    c = sprintf('%d',i);
    text(0.5+i,maprows+1+0.5,c,'FontSize',8,'Color','k');
    text(0.5+i,0.5,c,'FontSize',8,'Color','k');
end

% Put the comparison numbers in the picture itself.
title(sprintf('explored %d cells, shortest path %d cells (%d moves)', ...
    size(visited,1), size(steps,1), max(size(steps,1)-1,0)), 'Color','k');

% Off-screen patches purely to give the legend something to colour.
hwall = patch(NaN,NaN,wallcolour);
hexp  = patch(NaN,NaN,exploredcolour);
hpath = patch(NaN,NaN,pathcolour);
legend([hwall hexp hpath], {'Wall','Explored','Shortest path'}, ...
    'Location','eastoutside','TextColor','k','Color','w');

hold off

end



function placecell(y,x,colour)
% Fill one whole grid square.
rectangle('Position',[x,y,1,1],'FaceColor',colour);
end



function placeorder(y,x,i)
% Number an explored cell with its position in the exploration order.
c = sprintf('%d',i);
text(x+0.5,y+0.5,c,'FontSize',6,'Color',[0.15 0.25 0.45], ...
    'HorizontalAlignment','center','VerticalAlignment','middle');
end



function placestep(y,x,i,colour)
% Fill an inset square and number it, so the explored colour shows around it.
rectangle('Position',[x+0.1,y+0.1,0.8,0.8],'FaceColor',colour);
c = sprintf('%d',i);
text(x+0.5,y+0.5,c,'FontSize',7,'Color','k', ...
    'HorizontalAlignment','center','VerticalAlignment','middle');
end
