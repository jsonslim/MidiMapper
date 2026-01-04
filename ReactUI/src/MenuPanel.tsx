/*
    The action component represents a single action which happens after footswitch press
    Each FS can have multiple actions, which you choose from the menus
    The actions are storen in the array and execute in the order they are stored
*/

import './MenuPanel.css';
import { useState, useEffect } from 'react';
import DropDown from './DropDown';
import im from './assets/recycleBin.svg';  

export default function MenuPanel({ action }) {
    // console.log('action ', action);

    const [labels, setLabels] = useState([])
    const [actionState, setActionState] = useState({});
    const [currChoice, setCurrChoice] = useState({});

    let labelsArr = action[1].opts;

    useEffect(() => {
        const updatedState = { ...actionState, ...currChoice };
        console.log('updatedState ', updatedState);
        setActionState(updatedState);
    }, [currChoice]);

    function updateLabel(labelName) {

    }

    function deleteAction(e) {
        console.log('delete action ', e);
    }

    return (
        <>
            <div className="grid-container">
                {action.map((dd, i) => (
                    <DropDown
                        key={i}
                        opts={dd.opts}
                        name={dd.name}
                        getChoice={setCurrChoice}
                        onChange={updateLabel}
                    />
                ))}
                <img src={im} width='24' style={{
                    justifySelf: 'end',
                    alignSelf: 'end',
                    margin: '10px'
                }} onClick={deleteAction} />
            </div>
        </>
    )
}


