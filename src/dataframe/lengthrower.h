#include "rower.h"
#include "dataframe.h"

/** This class is a rower implementation. It calculates the total length of the row's elements
 * and stores it in the row's last element. For ints it calculates the number of digits. Floats are
 * rounded down to the nearest int and the number of digits are counted. Boolean adds 1 or 0, depending on
 * the value. Strings add the number of characters in the string */
class LengthRower : public Rower
{
public:
    DataFrame *df_;

    LengthRower(DataFrame* data) : df_(data) {};

    /** Counts the length of the row based on our strategy above and sets it to the last value of the row.
     * Therefore the last column of this row must be an int */
    bool accept(Row &r)
    {
        int size = 0;
        for (size_t i = 0; i < r.width() - 1; i++)
        {
            size += getSizeByType_(r, i);
        }

        df_->set(r.width() - 1, r.get_idx(), size);
    }

    /** Once traversal of the data frame is complete the rowers that were
      split off will be joined.  There will be one join per split. The
      original object will be the last to be called join on. The join method
      is reponsible for cleaning up memory. */
    void join_delete(Rower *other) {}

    /** This helper determines the type of the row, then returns the length of that element */
    size_t getSizeByType_(Row &r, size_t idx)
    {
        char colType = df_->get_schema().col_type(idx);
        switch (colType)
        {
            case 'I':
                return intSize_(r.get_int(idx));
            case 'B':
                return boolSize_(r.get_bool(idx));
            case 'F':
                return floatSize_(r.get_float(idx));
            case 'S':
                return stringSize_(r.get_string(idx));
            default:
                fprintf(stderr, "Data type %c not recognized\n",colType);
                exit(1);
        }
    }

    /** Return our size for an int */
    size_t intSize_(int val) {
        // referenced from https://www.geeksforgeeks.org/program-count-digits-integer-3-different-methods/
        size_t count = 0; 
        while (val != 0) {
            val = val / 10;
            count++;
        }

        return count;
    }

    /** Return size for a bool */
    size_t boolSize_(bool val) {
        //size 1 for true, 0 for false
        if (val) {
            return 1;
        }

        return 0;
    }

    /** Return size for a float */
    size_t floatSize_(float val) {
        int intVal = (int)val;

        return intSize_(intVal);
    }

    /**
     * Return size for a string */
    size_t stringSize_(String* val) {
        return val->size();
    }
};