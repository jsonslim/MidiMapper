import { useState } from 'react'
import './App.css'

import  MenuPanel  from "./MenuPanel"


function App() {
  const [count, setCount] = useState(0)

  return (
    <>   
      <MenuPanel action={action} updatePanelState={updatePanelState}></Action>/>
    </>
  )
}

export default App
