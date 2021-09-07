
// Throw away stream inputs. We use this for hiding debugging outputs in the 'dout' macro
// TODO: See if there is a better implementation to debugging prints, which still uses ostream syntax like cout.
class NullBuffer : public std::streambuf {
    public:
        int overflow(int c) { return c; }
};
NullBuffer null_buffer;
std::ostream g_nullout(&null_buffer);

// void operator<< (std::ostream& _os, StreamCommenter& _sc) {
//     _os << _sc.m_buffer.str() << std::endl;
// }
