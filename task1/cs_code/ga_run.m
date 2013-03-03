function results = ga_run(pm, xover, elitism)

pop_size = 80;
chrom_len = 20;
max_gen = 100;

results = zeros(10,3);

trials = 10;

for i=1:trials
    [gen,max_fit,mean_fit] = ga(pop_size, chrom_len, max_gen, pm, xover, elitism);
    results(i,1) = gen;
    results(i,2) = mean_fit;
    results(i,3) = max_fit;
end
end