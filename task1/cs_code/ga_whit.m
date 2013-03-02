function [P,best,result_gen,result_min_fit,result_mean_fit] = ga_whit(pop_size, chrom_len, pm, max_gen, elitism)
    %
    % Inputs:
    % pop_size  => population size
    % chrom_len => chromosome length
    % pm        => probability of mutation
    % pc        => probability of crossover
    % max_gen   => maximum number of generations
    %
    % Outputs:
    % P    => population
    % best => best individual of the population
    %
    % suggested run: [P,best] = ga(80, 20, pm, 100, elitism)


    % INITIALIZE POPULATION
    P = initialize(pop_size,chrom_len);

    % EVALUATION
    fit = whitley_fitness(P, chrom_len);
    
    [min_val, best_row] = min(fit);

    gen = 1;
    while gen<=max_gen
        % SELECTION
        [P, best_row] = tournament_selection(P, fit, 2, elitism, best_row);
        %[P, best_row] = roulette_selection(P, fit, elitism, best_row, chrom_len);

        % CROSSOVER
        %[P, best_row] = two_point_crossover(P, 0.5, elitism, best_row);
        %[P, best_row] = one_point_crossover(P,0.5, elitism, best_row);

        % MUTATION
        P = point_mutation(P,pm, elitism, best_row);
        
        % EVALUATION
        fit = whitley_fitness(P, chrom_len);

        % record data
        [min_fit(gen), best_row] = min(fit);
        mean_fit(gen) = mean(fit);

        % display information
        disp_info(gen,min_fit(gen),mean_fit(gen));
        
        result_gen = gen;
        gen = gen+1;
        result_min_fit = min_fit(end);
        result_mean_fit = mean_fit(end);
        if gen == 101
            gen = 100;
            break;
        end
    end

    disp(sprintf('FINAL Generation: %d',gen));
    disp(sprintf('FINAL Best fitness: %d\n',min_fit(end)));
    disp(sprintf('FINAL Mean fitness: %d\n',mean_fit(end)));

    % plot evolution curve
    plot(1:length(min_fit), min_fit,'b');
    hold on;
    plot(1:length(mean_fit), mean_fit,'g');
    hold off;
    xlabel('Generations');
    ylabel('Fitness');
    legend('Best fitness','Average fitness','Location','SouthEast');

    % output best individual
    [min_fit,ind] = min(fit);
    best = P(ind,:);
end

function [P] = initialize(pop_size,chrom_length)
    P = -10.24 + (10.24 - (-10.24)) .* rand(pop_size,chrom_length);
end

function num = gen_rand_num()
    num = -10.24 + (10.24 - (-10.24)) * rand;
end

function [fit] = maxones_fitness(P)
    for i=1:size(P,1)
        fit(i) = length(find(P(i,:)));
    end
    fit = chrom_len - fit;
end

function [fit] = whitley_fitness(P, chrom_len)
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

function [P_new, new_best_row] = two_point_crossover(P,pc, elitism, best_row)
    new_best_row = 1;
    mating_list = randperm(size(P,1));
    P_new = [];
    pair_best = 0;

    while ~isempty(mating_list)
        pair = mating_list(1:2);
        mating_list(1:2) = [];
 
        if pair(1) == best_row
            pair_best = 1;
        elseif pair(2) == best_row
            pair_best = 2;
        else
            pair_best = 0;
        end
 
        if rand<pc
            crossover_points = ceil(rand(1,2)*(size(P,2)));  
            point1 = min(crossover_points);
            point2 = max(crossover_points);
            individual1 = P(pair(1),:);
            individual2 = P(pair(2),:);
            individual1(point1:point2) = P(pair(2),point1:point2);
            individual2(point1:point2) = P(pair(1),point1:point2);
            if (elitism && (pair_best == 1))
                individual1 = P(best_row,:);
                new_best_row = size(P_new,1)+1;
            elseif (elitism && (pair_best == 2))
                individual2 = P(best_row,:);
                new_best_row = size(P_new,1)+2;
            end
            P_new = [P_new;individual1;individual2];
        else
            if (elitism && (pair_best == 1))
                new_best_row = size(P_new,1)+1;
            elseif (elitism && (pair_best == 2))
                new_best_row = size(P_new,1)+2;
            end
            P_new = [P_new;P(pair,:)];
        end
    end
end

function [P_new, new_best_row] = one_point_crossover(P,pc, elitism, best_row)
    new_best_row = 1;
    mating_list = randperm(size(P,1));
    P_new = [];
    while ~isempty(mating_list)
        pair = mating_list(1:2);
        mating_list(1:2) = [];
        if pair(1) == best_row
            pair_best = 1;
        elseif pair(2) == best_row
            pair_best = 2;
        else
            pair_best = 0;
        end
        if rand<pc
            crossover_point = ceil(rand(1,1)*(size(P,2)));
            individual1 = P(pair(1),:);
            individual2 = P(pair(2),:);
            individual1(1:crossover_point) = P(pair(2),1:crossover_point);
            individual2(1:crossover_point) = P(pair(1),1:crossover_point);
            if (elitism && (pair_best == 1))
                individual1 = P(best_row,:);
                new_best_row = size(P_new,1)+1;
            elseif (elitism && (pair_best == 2))
                individual2 = P(best_row,:);
                new_best_row = size(P_new,1)+2;
            end
            P_new = [P_new;individual1;individual2];
        else
            if (elitism && (pair_best == 1))
                new_best_row = size(P_new,1)+1;
            elseif (elitism && (pair_best == 2))
                new_best_row = size(P_new,1)+2;
            end
            P_new = [P_new;P(pair,:)];
        end
    end
end

function [P_new] = point_mutation(P,pm, elitism, best_row)
    r = rand(size(P));
    curr_best_row = P(best_row,:);
    mutation_list = find(r<pm);
    P_new = P;
    P_new(mutation_list(find(P(mutation_list)))) = gen_rand_num();
    P_new(mutation_list(find(P(mutation_list)))) = gen_rand_num();
    if elitism
        P_new(best_row,:) = curr_best_row;
    end
end

function [] = disp_info(gen, min_fit, mean_fit)
    if mod(gen,100)==0 
        disp(sprintf('Generation: %d',gen));
        disp(sprintf('Best min fitness: %d\n',min_fit));
        disp(sprintf('Best mean fitness: %d\n',mean_fit));
    end
end

function [P_new, new_best_row] = tournament_selection(P,fit,tourn_size, elitism, best_row)
    new_best_row = 1;
    for i=1:size(P,1)
        if elitism && (i == 1)
            P_new(1,:) = P(best_row,:);
            continue
        end
        pair = ceil(rand(1,tourn_size)*size(P,1));
        [min_fit,winner] = min(fit(pair));
        P_new(i,:) = P(pair(winner),:);
    end
end

function [P_new, new_best_row] = roulette_selection(P,fit, elitism, best_row, chrom_len)
    fit = chrom_len - fit;
    [fit, ind] = sort(fit);
    fit = (fit - min(fit)).^2;
    new_best_row = 1;
    if (fit == 0)
        P_new = P;
        return;
    end
    fit = cumsum(fit);
    fit = fit/max(fit);
    P_new = [];
    if elitism
        P_new(1,:) = P(best_row,:);
        for i=2:size(P,1)
            f = find(fit>rand);
            P_new = [P_new;P(ind(f(1)),:)];
        end
    else
        for i=1:size(P,1)
            f = find(fit>rand);
            P_new = [P_new;P(ind(f(1)),:)];
        end
    end
end