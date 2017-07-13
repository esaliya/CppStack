//
// Created by Saliya Ekanayake on 4/23/17.
//

#ifndef CLIONCPP_PARALLEL_OPS_H
#define CLIONCPP_PARALLEL_OPS_H

//extern int get_world_proc_rank;
//extern int get_world_procs_count;
//
//void initialize(int *argc, char ***argv);
//void teardown_parallelism();

class parallel_ops{
public:
    static parallel_ops initialize(int *argc, char ***argv);

    int get_world_proc_rank() const;
    int get_world_procs_count() const;

    void teardown_parallelism();


    ~parallel_ops();

private:
    int world_proc_rank;
    int world_procs_count;

    parallel_ops(int world_proc_rank, int world_procs_count);

};

#endif //CLIONCPP_PARALLEL_OPS_H
