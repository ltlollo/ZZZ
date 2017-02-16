#include "critto_satto.h"

using namespace std;


void check_argv::operator ()(Data& data)
{
    this->message = string{"Not enough parameters."
                    "\nUsage:\t./critto-satto {-e|-d|-a}"
                    " --key FILE --alg FILE [--in FILE] [--out FILE]\n"
                    "\n\t-e to encrypt.\n\t-d to decrypt.\n\t-a to generate the next alg."
                    "\n\t--alg to specify the algoritm file."
                    "\n\t--key to specify the key file."
                    "\n\t--in to specify an input file {Default is stdin}."
                    "\n\t--out to specify an input file {Default is stdout}."
                    "\n"};
    
    
    const string alg_flag{"--alg "};
    const string key_flag{"--key "};
    const string in_flag{"--in "};
    const string out_flag{"--out "};
    
    if (data.argc < 6)
        throw(error_id);
    
    for (size_t i{1}; i < data.argc; i++)
    {
        data.command.append(data.argv[i]);
        data.command.append(" ");
    }
    
    auto d_pos = data.command.find("-d ");
    auto e_pos = data.command.find("-e ");
    auto a_pos = data.command.find("-a ");
    
    if ((e_pos != string::npos)
        ^(d_pos != string::npos)
        ^(a_pos != string::npos))
    {
        if (d_pos != string::npos)
            data.encryption_flag = false;
        else data.encryption_flag = true;
        
        if (a_pos != string::npos)
        {
            data.alg_on_alg_flag = true;
            data.times = 1000;
        }
        else data.alg_on_alg_flag = false;
    }
    else throw(error_id);
    
    auto alg_pos = data.command.find(alg_flag);
    auto key_pos = data.command.find(key_flag);
    
    if (alg_pos != string::npos && key_pos != string::npos)
    {
        data.alg_file = util_get_arg(data.command, alg_flag, alg_pos);
        data.key_file = util_get_arg(data.command, key_flag, key_pos);
        if (data.alg_file.empty() || data.key_file.empty())
            throw(error_id);
    }
    else throw(error_id);
    
    auto in_pos = data.command.find(in_flag);
    auto out_pos = data.command.find(out_flag);
    
    if (in_pos != string::npos)
    {
        data.msg_file = util_get_arg(data.command, in_flag, in_pos);
        data.work_on_stdin = false;
        if (data.msg_file.empty())
            throw(error_id);
    }
    else data.work_on_stdin = true;
    if (out_pos != string::npos)
    {
        data.out_file = util_get_arg(data.command, out_flag, out_pos);
        data.work_on_stdout = false;
        
        if (data.out_file.empty())
            throw(error_id);
    }
    else data.work_on_stdout = true;
}

void grab_stdin::operator ()(Data& data)
{
    if (data.work_on_stdin)
    {
        cin >> noskipws;
        istream_iterator<char> it(cin);
        istream_iterator<char> end;
        data.content = string(it, end);
    }
}


void read_file::operator ()(Data& data)
{    
    auto manage_err = [&](int err, const string& fname){
        switch (err) {
        case -1:
            break;
        case 0:
            this->message = "Sizes don't match.";
            throw(error_id);
        case 1:
            this->message = fname + " empty"
                            " or size not multiple of " + to_string(N_CHARS);
            throw(error_id);
        case 2:
            this->message = "Unable to find or open file " + fname;
            throw(error_id);
        default:
            this->message = "Do not implement.";
            throw(error_id);
        }
    };

    int err = util_read_file(data.alg_file, data.alg);
    manage_err(err, data.alg_file);

    if (data.alg.size() < 24)
    {
        this->message = data.alg_file + " is too small.";
        cerr << "WARNING: " << this->message << endl;
    }

    err = util_read_file(data.key_file, data.key);
    manage_err(err, data.key_file);
}


void random_pop::operator ()(Data& data)
{
    if (!data.alg_on_alg_flag)
    {
        data.pop = data.alg.back();
        data.guard = data.alg.back();
        
        vector<op_t> v_op;
        v_op.reserve(data.alg.size()*N_CHARS/4);
        alg_to_op_vector(data.alg, v_op);
        
        apply_rounds(data.pop, data.key.front(),
                     v_op, data.times*3,true, true
                     );
        apply_rounds(data.guard, data.key.front(),
                     v_op, data.times*7,true, true
                     );
    }
}


void read_stdin::operator ()(Data& data)
{
    if (data.work_on_stdin)
    {
        if (data.content.empty())
        {
            this->message = "Missing stdin somehow.";
            throw(error_id);
        }
        
        if ((data.alg_on_alg_flag || !data.encryption_flag)
            && (data.content.size()%N_CHARS != 0))
        {
            this->message = "If decrypting something or operating on algoritm,"
                            " input has to be multiple of " + to_string(N_CHARS);
            throw(error_id);
        }
        
        data.msg.reserve(data.content.size()/N_CHARS + 1);
        array<unsigned char, N_CHARS> u_array;

        auto it2 = u_array.begin();
        for (auto it1 = data.content.cbegin(); it1 != data.content.cend();
             ++it1, ++it2
             )
        {
            if (it2 == u_array.end())
            {
                data.msg.push_back(u_array);
                it2 = u_array.begin();
            }
            *it2 = *it1;
        }

        if (it2 != u_array.end())
        {
            auto it_pop = data.pop.begin();
            for (; it2 != u_array.end(); ++it2, ++it_pop)
                *it2 = *it_pop;
            data.msg.push_back(u_array);
        }
        else
        {
            data.msg.push_back(u_array);
            if (!data.alg_on_alg_flag && data.encryption_flag)
                data.msg.push_back(data.guard);
        }
        data.msg.shrink_to_fit();
    }
}


void read_message::operator ()(Data& data)
{
    if (!data.work_on_stdin)
    {
        if (data.alg_on_alg_flag && data.alg_file == data.msg_file)
        {
            data.msg.reserve(data.alg.size());
            for (auto it = data.alg.cbegin();it != data.alg.cend(); ++it)
                data.msg.push_back(*it);
        }
        else
        {
            size_t size;
            ifstream file(data.msg_file, ios::in|ios::binary|ios::ate);
            array<unsigned char, N_CHARS> u_array;

            if (!file.is_open())
            {
                this->message = "Unable to find or open file " + data.msg_file;
                throw(error_id);
            }

            size = file.tellg();
            file.seekg(0, ios::beg);

            if (!(size > 0))
            {
                this->message = data.msg_file + " is empty.";
                throw(error_id);
            }
            else if ((data.alg_on_alg_flag || !data.encryption_flag)
                     && size % N_CHARS != 0)
            {
                this->message = "If decrypting something"
                                " or operating on algoritm, input"
                                " has to be multiple of " + to_string(N_CHARS);
                throw(error_id);
            }
            else
            {
                data.msg.reserve(size/N_CHARS + 1);
                mmask = new char [size];
                file.read(mmask, size);

                auto it = u_array.begin();
                for (size_t i{0}; i < size; ++i, ++it)
                {
                    if (it == u_array.end())
                    {
                        data.msg.push_back(u_array);
                        it = u_array.begin();
                    }
                    *it = (unsigned char)mmask[i];
                }
                if (it != u_array.end())
                {
                    auto it_pop = data.pop.begin();
                    for (; it != u_array.end(); ++it, ++it_pop)
                        *it = *it_pop;
                    data.msg.push_back(u_array);
                }
                else
                {
                    data.msg.push_back(u_array);
                    if (!data.alg_on_alg_flag && data.encryption_flag)
                        data.msg.push_back(data.guard);
                }
                data.msg.shrink_to_fit();
            }
        }
    }
}


void enc_stub::operator ()(Data& data)
{
    if (data.encryption_flag)
    {
        if (data.key.size() >= data.msg.size() &&
            data.key.size() >= data.alg.size())
        {
            auto it_k = data.key.cbegin();
            for (auto it_m = data.msg.begin();
                 it_m != data.msg.end();
                 ++it_m, ++it_k
                 )
            {
                vector<op_t> v_op;
                v_op.reserve(data.alg.size()*N_CHARS/4);
                alg_to_op_vector(data.alg, v_op);

                apply_rounds(*it_m, *it_k, v_op,
                             data.times, true, data.alg_on_alg_flag
                             );

                for (size_t i{0}; i < data.alg.size(); ++i)
                    apply_rounds(data.alg.at(i), data.key.at(i),
                                 v_op, data.distance, true, true
                                 );
            }
        }
        else
        {
            this->message = "Key file has to be bigger than"
                            " alg file and message.";
            throw(error_id);
        }
    }
}

void dec_stub::operator ()(Data& data)
{
    if (!data.encryption_flag)
    {
        if (data.key.size() >= data.msg.size() &&
            data.key.size() >= data.alg.size())
        {
            auto it_k = data.key.cbegin();
            for (auto it_m = data.msg.begin();
                 it_m!=data.msg.end();
                 ++it_m, ++it_k
                 )
            {
                vector<op_t> v_op;
                v_op.reserve(data.alg.size()*N_CHARS/4);
                alg_to_op_vector(data.alg, v_op);
                vector<op_t> v_op_rev;
                v_op_rev.reserve(v_op.size());
                for (auto rit = v_op.crbegin(); rit != v_op.crend(); ++rit)
                    v_op_rev.push_back(*rit);

                apply_rounds(*it_m, *it_k, v_op_rev,
                             data.times, false, false
                             );

                for (size_t i{0}; i < data.alg.size(); ++i)
                    apply_rounds(data.alg.at(i), data.key.at(i),
                                 v_op, data.distance, true, true
                                 );
            }

            if (data.guard == data.msg.back())
            {
                data.msg.pop_back();
                data.message_end_pos = N_CHARS;
            }
            else
            {
                array<unsigned char, N_CHARS> f = data.msg.back();
                bool found = false;
                auto it1 = f.begin();

                while (!found)
                {
                    found = true;
                    while (*it1 != *data.pop.begin() && it1 != f.end())
                        ++it1;

                    if (it1 == f.end())
                    {
                        this->message = "The message is non conformant"
                                        " or corrupted.";
                        throw(error_id);
                    }

                    auto it2 = data.pop.begin();
                    auto it1_ = it1;

                    while ( it1_ != f.end() && found)
                    {
                        if (*it1_ != *it2) found = false;
                        ++it1_; ++it2;
                    }
                    if (!found) ++it1;
                }
                data.message_end_pos = distance(f.begin(), it1);
            }
        }
        else
        {
            this->message = "Key file has to be bigger"
                            " than alg file and message.";
            throw(error_id);
        }
    }
}

void write_output::operator ()(Data& data)
{
    if (!data.work_on_stdout)
    {
        ofstream file(data.out_file, ios::in);

        if (file.is_open())
        {
            this->message = data.out_file + " already exists.";
            throw(error_id);
        }

        file.open(data.out_file, ios::out | ios::binary);

        file.exceptions(ofstream::failbit | ofstream::badbit);

        if (!file.is_open())
        {
            this->message = "Unable to find or open file " + data.out_file;
            throw(error_id);
        }

        for (auto it1 = data.msg.begin(); it1 != data.msg.end()-1; ++it1)
        {
            for (auto it2: *it1) file << it2;
        }
        if (!data.encryption_flag)
        {
            for (size_t i{0}; i < data.message_end_pos; ++i)
            {
                file << data.msg.back()[i];
            }
        }
        else for (auto it: data.msg.back()) file << it;
    }
}

void output_message::operator ()(Data& data)
{
    if (data.work_on_stdout)
    {
        for (auto it1 = data.msg.begin(); it1 != data.msg.end()-1; ++it1)
        {
            for (auto it2: *it1) cout << it2;
        }
        if (!data.encryption_flag)
        {
            for (size_t i{0}; i < data.message_end_pos; ++i)
            {
                cout << data.msg.back()[i];
            }
        }
        else
        {
            for (auto it: data.msg.back())  cout << it;
        }
    }
}
