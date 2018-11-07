#ifndef tb_transaction_writer_h
#define tb_transaction_writer_h

#include <cstdio>
#include <string>

namespace tb {

/* Lazy transaction file writer
 *
 * tb::transaction_writer writer("sample.dat");
 * writer.append(42);
 * writer.append(0xbeef);
 *
 * This crates a transaction file "sample.dat" with the following content:
 *
 * [[[runtime]]]
 * [[transaction]] 0
 * 0x2a
 * [[/transaction]]
 * [[transaction]] 1
 * 0xbeef
 * [[/transaction]]
 * [[[/runtime]]]
 */
class transaction_writer
{
protected:
    FILE* fp_{nullptr};
    std::string format_{"0x%x"}; // default format is plain hex with base
    size_t count_{0};
public:
    transaction_writer(const std::string& filename)
    {
        fp_ = fopen("charge_corr.dat", "w");
        fprintf(fp_, "[[[runtime]]]\n");
    }
    virtual ~transaction_writer()
    {
        fprintf(fp_, "[[[/runtime]]]\n");
        fclose(fp_);
    }
    /* Set transaction value format */
    void set_format(const std::string& format)
    {
        format_ = format;
    }
    template<typename T>
    void append(const T value)
    {
        fprintf(fp_, "[[transaction]] %d\n", count_);
        fprintf(fp_, format_.c_str(), value);
        fprintf(fp_, "\n[[/transaction]] \n");
        ++count_;
    }
};

} // namespace tb

#endif
