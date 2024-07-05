#pragma once


struct Line
{
    int first;
    int second;
    int count = 0;
    Line(int f, int s) : first(f), second(s) {}
    Line() = delete;
    bool operator==(const Line& l) const
    {
        return (first == l.first && second == l.second) || (first == l.second && second == l.first);
    }
    bool operator!=(const Line& l) const
    {
        return (first != l.first || second != l.second) && (first != l.second || second != l.first);
    }
    bool operator<(const Line& l) const
    {
        return first < l.first || (first == l.first && second < l.second);
    }
    bool operator>(const Line& l) const
    {
        return first > l.first || (first == l.first && second > l.second);
    }
    bool operator<=(const Line& l) const
    {
        return first <= l.first || (first == l.first && second <= l.second);
    }
    bool operator>=(const Line& l) const
    {
        return first >= l.first || (first == l.first && second >= l.second);
    }
};

