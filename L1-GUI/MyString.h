//
// Created by mhq on 24/05/23.
//
#pragma once

#include <QStringList>
#include "../L1/graphs.h"

template< typename Cont >
QString showUnoriented(const Cont& path) {
    QStringList list;
    for (auto x : path) {
        list.append({static_cast<char>('a' + x)});
    }
    return list.join("–");
}

template< typename Cont >
QString showOriented(const Cont& path) {
    QStringList list;
    for (auto x : path) {
        list.append({static_cast<char>('a' + x)});
    }
    return list.join("→");
}