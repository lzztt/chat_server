/* 
 * File:   DataFrameParser.hpp
 * Author: ikki
 *
 * Created on March 10, 2015, 10:02 PM
 */

#ifndef DATAFRAMEPARSER_HPP
#define	DATAFRAMEPARSER_HPP

class DataFrameParser
{
public:
    explicit DataFrameParser();

    DataFrameParser(const DataFrameParser& other) = delete;
    DataFrameParser& operator=(const DataFrameParser& other) = delete;

    DataFrameParser(DataFrameParser&& other);
    DataFrameParser& operator=(DataFrameParser&& other);

    ~DataFrameParser();
};

#endif	/* DATAFRAMEPARSER_HPP */

