template<bool condition>
struct If {
    template<typename T>
    static constexpr T then(T implication_true, T implication_false) {
        return implication_true;
    }
};

template<>
struct If<false> {
    template<typename T>
    static constexpr T then(T implication_true, T implication_false) {
        return implication_false;
    }
};



template<int xorsum, int cur_heap, int... other_heaps>
struct ChooseTurn;

template<int cur_heap, int xorsum, int heap_size, int... other_heaps>
struct ChooseTurn<cur_heap, xorsum, heap_size, other_heaps...> {
    static constexpr int heap_num =
            If <(heap_size - (xorsum ^ heap_size) >= 0)>
                    ::then(cur_heap, ChooseTurn<xorsum, cur_heap + 1, other_heaps...>::heap_num);
    static constexpr int need_to_take =
            If<heap_size - (xorsum ^ heap_size) >= 0>
                    ::then(heap_size - (xorsum ^ heap_size), ChooseTurn<xorsum, cur_heap + 1, other_heaps...>::need_to_take);
};

template<int xorsum, int cur_heap>
struct ChooseTurn<xorsum, cur_heap> {
    static constexpr int heap_num = 0, need_to_take = 0;
};



template<int xorsum, int... heaps>
struct Answer;

template<int xorsum, int cur_heap, int... other_heaps>
struct Answer<xorsum, cur_heap, other_heaps...> {
    static constexpr int who_won = Answer<xorsum ^ cur_heap, other_heaps...>::who_won;
    static constexpr int final_xorsum = Answer<xorsum ^ cur_heap, other_heaps...>::final_xorsum;
};

template<int xorsum>
struct Answer<xorsum> {
    static constexpr int who_won = If <xorsum>::then(1, 1 + 1);
    static constexpr int final_xorsum = xorsum;
};



template<int... heaps>
struct AhalaiMahalai {
    static constexpr int who = Answer<0, heaps...>::who_won;
    static constexpr int whence = If<who == 1>
            ::then(ChooseTurn<1, Answer<0, heaps...>::final_xorsum, heaps...>::heap_num, 0);
    static constexpr int how = If<who == 1>
            ::then(ChooseTurn<1, Answer<0, heaps...>::final_xorsum, heaps...>::need_to_take, 0);
};