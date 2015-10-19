﻿using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a color. Color is displayed as a GUI color field that allows
    /// the user to manipulate the color using a color picker.
    /// </summary>
    public class InspectableColor : InspectableField
    {
        private GUIColorField guiField;
        private InspectableState state;

        /// <summary>
        /// Creates a new inspectable color GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableColor(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, SerializableProperty.FieldType.Color, depth, layout, property)
        {

        }

        /// <inheritoc/>
        protected internal override void Initialize(int layoutIndex)
        {
            if (property != null)
            {
                guiField = new GUIColorField(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }
        }

        /// <inheritdoc/>
        public override InspectableState Refresh(int layoutIndex)
        {
            if (guiField != null)
                guiField.Value = property.GetValue<Color>();

            InspectableState oldState = state;
            if (state.HasFlag(InspectableState.Modified))
                state = InspectableState.NotModified;

            return oldState;
        }

        /// <summary>
        /// Triggered when the user selects a new color.
        /// </summary>
        /// <param name="newValue">New value of the color field.</param>
        private void OnFieldValueChanged(Color newValue)
        {
            property.SetValue(newValue);
            state = InspectableState.Modified;
        }
    }
}
