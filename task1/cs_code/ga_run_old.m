function results = ga_run()
    % Perform 18 experiments using Maxones fitness function.
    % vary mutation rate, crossover, and elitism.
    % aggregate results, save and plot.

    pop_size = 80;
    chrom_len = 20;
    max_gen = 100;

    mut_rates = [0.1, 0.01, 0.001];
    crossovers = [0, 1, 2];
    elitism = true;

    trials = 10;
    
    csv_row = 0;
    datafile = 'maxones_results.csv';
    %fid = fopen(datafile, 'wt');

    %%%%%%%%%%%%%%
    % Elitism ON %
    %%%%%%%%%%%%%%

    for i=1:size(crossovers,2)
        for j=1:size(mut_rates,2)
            results = zeros(12,3);
            xover = crossovers(i);
            mut_rate = mut_rates(j);
            label = ['elitism: on ', 'crossover: ', num2str(xover), ', mut_rate: ', num2str(mut_rate)];
            %'\n', 'Generation,', 'max fit,', 'mean fit,'];
            xlswrite(datafile, label);
            %fprintf(fid, label);
            %fprintf(fid, '\n');
            %cols = ['Generation', 'max fit', 'mean fit'];
            %for m=1:3
            %    fprintf(fid, cols(m));
            %    fprintf(fid, '\t');
            %end
            %fprintf(fid, '\n');
            
            % generate results for ten trials
            for k=1:10
                [gen,max_fit,mean_fit] = ga(pop_size, chrom_len, max_gen, mut_rate, xover, elitism);
                results(k,1) = gen;
                results(k,2) = mean_fit;
                results(k,3) = max_fit;
                
                % write results to file
                %fwrite(fid, results(k,:));
            end
            
            
        end
    end
    %fclose(fid);

end