function results = ga_w_run(pop_size, chrom_len, pm, max_gen, elitism)

results = zeros(10,3);

trials = 10;

for i=1:trials
    [P,best,gen,min_fit,mean_fit] = ga_whit(pop_size, chrom_len, pm, max_gen, elitism);
    results(i,1) = gen;
    results(i,2) = mean_fit;
    results(i,3) = min_fit;
end
end