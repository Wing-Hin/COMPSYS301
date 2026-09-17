%% This is a shell that you will have to follow strictly. 
% You will use the plotmap() and viewmap() to display the outcome of your algorithm.

% Load sample_data_map_8, three variables will be created in your workspace. These were created as a 
% result of [m,v,s]=dfs('map_8.txt',[14,1],[1,18]);
% The solution can be viewed using 
% plotmap(m,s) 

% write your own function for the DFS algorithm.
function [retmap, retvisited, retsteps] = dfs(mapfile, startlocation, targetlocation)
    retmap = map_convert(mapfile); 
    [rows, cols] = size(retmap);
    retvisited = zeros(rows, cols); % tracks visited nodes
    retsteps = [];
    directions = [-1, 0; 1, 0; 0, -1; 0, 1]; % up, down, left, right
    history = {startlocation};
    
    while ~isempty(history) 
        % LIFO pop
        current_path = history{end}; % get the last path added
        history(end) = []; % remove
        curr_node = current_path(end, :); % get the last position in pathway
        r = curr_node(1);
        c = curr_node(2);
        
        if r == targetlocation(1) && c == targetlocation(2) % check win
            retvisited(r, c) = 1;
            retsteps = current_path;
            return;
        end
        
        if retvisited(r, c) == 0
            retvisited(r, c) = 1;
            
            for d = 1:4
                next_r = r + directions(d, 1);
                next_c = c + directions(d, 2);
                
                if next_r >= 1 && next_r <= rows && next_c >= 1 && next_c <= cols % boundary check
                    
                    % Check B: Is it a valid path? (0 = path, 1 = wall/obstacle)
                    % Check C: Has it not been visited yet? (0 = unvisited)
                    if retmap(next_r, next_c) == 0 && retvisited(next_r, next_c) == 0
                        
                        new_path = [current_path; next_r, next_c]; % add neigbour to current path history and push
                        history{end+1} = new_path; 
                    end
                end
            end
        end
    end
    
    % If the while loop completely empties the stack and hasn't returned, the target is unreachable
    disp('No path found from start to target.');
end

function placestep(position,i)
% This function will plot a insert yellow rectangle and also print a number in this rectangle. Use with plotmap/viewmap. 
position = [16-position(1) position(2)];
position=[position(2)+0.1 position(1)+0.1];
rectangle('Position',[position,0.8,0.8],'FaceColor','y');
c=sprintf('%d',i);
text(position(1)+0.2,position(2)+0.2,c,'FontSize',10);
end