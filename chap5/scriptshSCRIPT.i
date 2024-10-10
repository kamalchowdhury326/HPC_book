 
============================================================
=====     Queued job information at submit time        =====
============================================================
  The submitted file is: script.sh
  The time limit is 4:00:00 HH:MM:SS.
  The target directory is: /home/ualmkc001/HPC_book/chap5
  The memory limit is: 500mb
  The job will start running after: 202410091729.52
  Job Name: scriptshSCRIPT
  Queue: -q express
  Constraints: 
  Command typed:
/scripts/run_script script.sh     
  Queue submit command:
qsub -q express -j oe -N scriptshSCRIPT -a 202410091729.52 -r n -M mhchowdhury@crimson.ua.edu -l walltime=4:00:00 -l select=ncpus=8:mpiprocs=8:mem=500mb 
  Job number: 
 
