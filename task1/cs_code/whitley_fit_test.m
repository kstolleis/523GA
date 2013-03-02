function [fit] = whitley_fitness(P)
    for n = 1:size(P,1)
        I = P(n,:);
        temp = 0;
        for i = 1:size(P,2)
            for j = 1:size(P,2)
                temp = temp + ((((100 * (I(i)^2 - I(j))^2 + (1 - I(j))^2)^2)/4000)...
                    - cos(100 * (I(i)^2 - I(j))^2 + (1 - I(j))^2) + 1);
            end
        end
        fit(n) = temp;
    end
end