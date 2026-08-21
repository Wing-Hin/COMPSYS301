%% This is a shell that you will have to follow strictly.
% You will use the plotmap() and viewmap() to display the outcome of your algorithm.

% Load sample_data_map_8, three variables will be created in your workspace. These were created as a
% result of [m,v,s]=bfs('map_8.txt',[14,1],[1,18]);
% The solution can be viewed using
% plotmap(m,s)

function [retmap,retvisited,retsteps] = algo_test(mapfile,startlocation,targetlocation,algorithm)
% The fourth argument chooses the search: 'bfs' (default) or 'astar'.
if nargin < 4
    algorithm = 'bfs';
end

% Locate the Python script next to this .m file so it works from any folder.
scriptdir = fileparts(mfilename('fullpath'));
pyscript  = fullfile(scriptdir, 'algorithm_test.py');

% If the map file isn't on the current path, look next to this .m file too.
if ~isfile(mapfile)
    mapfile = fullfile(scriptdir, mapfile);
end

% Find a Python interpreter.
pyexe = fullfile(getenv('USERPROFILE'), 'anaconda3', 'python.exe');
if ~isfile(pyexe)
    pyexe = 'python';
end

% MATLAB is 1-based, Python is 0-based: subtract 1 on the way out.
cmd = sprintf('"%s" "%s" "%s" %d %d %d %d --algorithm %s', ...
    pyexe, pyscript, mapfile, ...
    startlocation(1)-1, startlocation(2)-1, ...
    targetlocation(1)-1, targetlocation(2)-1, algorithm);

[status, out] = system(cmd);
if status ~= 0
    error('bfs:pythonFailed', 'Python call failed:\n%s', out);
end

result = jsondecode(out);

retmap = result.map;

if ~result.found
    warning('bfs:noPath', 'No path found between the given locations.');
    retvisited = [];
    retsteps   = [];
    return
end

% Add 1 on the way back in (Python 0-based -> MATLAB 1-based).
retvisited = result.explored + 1;
retsteps   = result.path + 1;

end



function placestep(position,i)
% This function will plot a insert yellow rectangle and also print a number in this rectangle. Use with plotmap/viewmap.
position = [16-position(1) position(2)];
position=[position(2)+0.1 position(1)+0.1];
rectangle('Position',[position,0.8,0.8],'FaceColor','y');
c=sprintf('%d',i);
text(position(1)+0.2,position(2)+0.2,c,'FontSize',10);
end
