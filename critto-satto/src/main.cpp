#include "macchina_antani.h"
#include "critto_satto.h"

using namespace stmac;
using namespace std;

int main(int argc,char *argv[])
{
    GlobalScope<Data> UltimateMachine;
    check_argv s0;
    grab_stdin s1;
    read_file s2;
    random_pop s3;
    read_stdin s4;
    read_message s5;
    enc_stub s6;
    dec_stub s7;
    write_output s8;
    output_message s9;

    UltimateMachine.getInput(argc, argv);
    UltimateMachine.addState(&s0);
    UltimateMachine.addState(&s1);
    UltimateMachine.addState(&s2);
    UltimateMachine.addState(&s3);
    UltimateMachine.addState(&s4);
    UltimateMachine.addState(&s5);
    UltimateMachine.addState(&s6);
    UltimateMachine.addState(&s7);
    UltimateMachine.addState(&s8);
    UltimateMachine.addState(&s9);

    try
    {
        UltimateMachine.run();
    }
    catch (exception& zefuck)
    {
        cerr << "ERROR: std::exception occurred: " << zefuck.what() << endl;
    }
    catch (...)
    {
        cerr << "ERROR: Internal or unknown exception occurred" << endl;
    }
}

