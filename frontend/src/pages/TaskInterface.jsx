import React, { useState } from 'react';
import { Calendar, BrainCircuit, FolderTree } from 'lucide-react';
import Card from '../components/ui/Card'; // Adjust the import path as necessary

const TaskInterface = () => {
  const [activeTab, setActiveTab] = useState('tasks');

  return (
    <div className="h-screen flex">
      {/* Left Sidebar - Navigation */}
      <div className="w-16 bg-gray-900 flex flex-col items-center py-4 space-y-8">
        <button onClick={() => setActiveTab('tasks')} className={`p-2 rounded-lg ${activeTab === 'tasks' ? 'bg-blue-600' : 'hover:bg-gray-800'}`}>
          <Calendar className="w-6 h-6 text-white" />
        </button>
        <button onClick={() => setActiveTab('knowledge')} className={`p-2 rounded-lg ${activeTab === 'knowledge' ? 'bg-blue-600' : 'hover:bg-gray-800'}`}>
          <BrainCircuit className="w-6 h-6 text-white" />
        </button>
        <button onClick={() => setActiveTab('files')} className={`p-2 rounded-lg ${activeTab === 'files' ? 'bg-blue-600' : 'hover:bg-gray-800'}`}>
          <FolderTree className="w-6 h-6 text-white" />
        </button>
      </div>

      {/* Main Content Area */}
      <div className="flex-grow p-4">
        {activeTab === 'tasks' && (
          <div>
            {/* Task List View */}
            <h2 className="font-semibold text-lg mb-4">Tasks</h2>
            <Card className="p-3">Task 1</Card>
            <Card className="p-3">Task 2</Card>
            {/* More tasks can be dynamically listed here */}
          </div>
        )}
        {activeTab === 'knowledge' && (
          <div>
            {/* Concept Map Placeholder */}
            <h2 className="font-semibold text-lg mb-4">Knowledge Map</h2>
            <p>Concept map will be displayed here.</p>
          </div>
        )}
        {activeTab === 'files' && (
          <div>
            {/* File Directory Structure */}
            <h2 className="font-semibold text-lg mb-4">Files</h2>
            <p>File directory functionality will be implemented here.</p>
          </div>
        )}
      </div>
    </div>
  );
};

export default TaskInterface;
