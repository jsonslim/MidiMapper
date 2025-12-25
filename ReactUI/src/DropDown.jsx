import { useState, useContext } from "react";
import Select from 'react-select'
import './DropDown.css'

const bgColor = '#dde1e7';
const customStyles = {
    control: (provided, state) => ({
        ...provided,
        backgroundColor: bgColor,
        border: 'none',
        boxShadow: 'none',
    }),
    menu: (provided) => ({
        ...provided,
        backgroundColor: bgColor,
        border: '1px solid grey'
    }),
    option: (provided, state) => ({
        ...provided,
        backgroundColor: state.isFocused ? bgColor : bgColor,
        color: '#575757',
    }),
    singleValue: (provided) => ({
        ...provided,
        color: '#575757',
    }),
    input: (provided) => ({
        ...provided,
        color: 'grey', // cursor color
    }),
    dropdownIndicator: (provided) => ({
        ...provided,
        color: 'grey',
    }),
    // hide the indicator separator
    indicatorSeparator: () => ({
        display: 'none',
    }),
}

export default function DropDown({ opts, name, getChoice }) {
    const [selected, setSelected] = useState('');

    opts = opts.map((option) => {
        return { value: option, label: option }
    });

    function changeHandler(e) {
        setSelected(e.value);
        getChoice({ [name]: e.value });
    }

    return (
        <>
            <div className="dropdown-container">
                <label className="dropdown-label">{name}</label>
                <Select
                    styles={customStyles}
                    value={{ label: selected }} // makes it controlled
                    options={opts}
                    onChange={(e) => changeHandler(e)}
                ></Select>
            </div >
        </>
    )
}