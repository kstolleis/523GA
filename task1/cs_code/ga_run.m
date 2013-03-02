function results = ga_run(pop_size, chrom_len, pm, max_gen, elitism)

results = zeros(10,3);

trials = 10;

for i=1:trials
    [P,best,gen,max_fit,mean_fit] = ga(pop_size, chrom_len, pm, max_gen, elitism);
    results(i,1) = gen;
    results(i,2) = mean_fit;
    results(i,3) = max_fit;
end
end